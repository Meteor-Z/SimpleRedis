#include "hash_table.h"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <string>

namespace SimpleRedis {

struct Entry {
    HNode m_node {};
    std::string m_key {};
    std::string m_value {};
};

HTab::HTab(const size_t n) {
    // n是 2^n 的倍数
    assert(n > 0 && ((n - 1) & n) == 0);
    m_tab = (HNode**)calloc(sizeof(HNode*), n);
    m_mask = n - 1;
    m_size = 0;
}

void HTab::insert(HNode* node) {
    size_t pos = node->m_code & m_mask;
    HNode* next = m_tab[pos];
    node->m_next = next;
    m_tab[pos] = node;
    m_size++;
}
HNode** HTab::lookup(HNode* key, Cmp cmp) {
    if (!m_tab) {
        return nullptr;
    }
    // 很简单, 直接传入
    size_t pos = key->m_code & m_mask;
    HNode** from = &m_tab[pos];
    for (HNode* cur; (cur = *from) != nullptr; from = &cur->m_next) {
        if (cur->m_code == key->m_code && cmp(cur, key)) {
            return from;
        }
    }

    return nullptr;
}

HNode* HTab::detach(HNode** from) {
    HNode* node = *from;
    *from = node->m_next;
    m_size--;
    return node;
}

void HMap::insert(HNode* node) {
    if (!m_tab1.m_tab) {
        m_tab1 = HTab { 4 };
    }
    m_tab1.insert(node);

    if (!m_tab2.m_tab) {
        // 如果第一个tab中的负载比较大的话，开始准备调整，
        size_t load_factor = m_tab1.m_size / (m_tab1.m_mask + 1);
        if (load_factor > MAX_LOAD_FACTOR) {
            start_resizing();
        }
    }

    help_resizing();
}

HNode* HMap::lookup(HNode* node, Cmp cmp) {
    // 从两张表中查找
    help_resizing();
    HNode** from = m_tab1.lookup(node, cmp);
    from = from ? from : m_tab2.lookup(node, cmp);
    return from ? *from : nullptr;
}

void HMap::start_resizing() {
    assert(m_tab2.tab == nullptr);
    m_tab2 = m_tab1;
    m_tab1 = HTab { (m_tab1.m_mask + 1) * 2 };
    m_resizing_pos = 0;
}

void HMap::help_resizing() {
    size_t work { 0 };
    // 将旧表放到新表上面，
    while (work < MAX_RESIZING_WORK && m_tab2.m_size > 0) {
        HNode** from = &m_tab2.m_tab[m_resizing_pos];
        if (!*from) {
            m_resizing_pos++;
            continue;
        }
        m_tab1.insert(m_tab2.detach(from));
        work++;
    }

    if (m_tab2.m_size == 0 && m_tab2.m_tab) {
        free(m_tab2.m_tab);
        m_tab2 = HTab {};
    }
}
} // namespace SimpleRedis