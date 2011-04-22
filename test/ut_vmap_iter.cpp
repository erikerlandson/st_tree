#include <boost/test/unit_test.hpp>

#include "ootree.h"

using namespace ootree;
using namespace std;

BOOST_AUTO_TEST_SUITE(ut_valmap_iterator_adaptor)

template <typename Iterator, typename ValMap>
struct valmap_iterator_adaptor : public valmap_iterator_dispatch<Iterator, ValMap, typename Iterator::iterator_category>::adaptor_type {
    typedef typename valmap_iterator_dispatch<Iterator, ValMap, typename Iterator::iterator_category>::adaptor_type adaptor_type;

    typedef typename adaptor_type::iterator_category iterator_category;
    typedef typename adaptor_type::value_type value_type;
    typedef typename adaptor_type::difference_type difference_type;
    typedef typename adaptor_type::pointer pointer;
    typedef typename adaptor_type::reference reference;

    typedef Iterator base_iterator;

    valmap_iterator_adaptor() : adaptor_type() {}
    virtual ~valmap_iterator_adaptor() {}
    
    explicit valmap_iterator_adaptor(const valmap_iterator_adaptor& src) : adaptor_type(src) {}
    valmap_iterator_adaptor& operator=(const valmap_iterator_adaptor& src) {
        if (this == &src) return *this;
        adaptor_type::operator=(src);
        return *this;
    }

    operator base_iterator() const { return this->_base; }
    valmap_iterator_adaptor(const base_iterator& src) : adaptor_type(src) {}
    valmap_iterator_adaptor& operator=(const base_iterator& src) {
        adaptor_type::operator=(src);
        return *this;
    }
};

BOOST_AUTO_TEST_CASE(dref_map) {
    int* t1 = new int(4);
    dref_vmap<int*> vmap;
    BOOST_CHECK_EQUAL(vmap(t1), *t1);
    delete t1;
}

BOOST_AUTO_TEST_CASE(dref_map_const) {
    const int* t1 = new int(4);
    dref_vmap<const int*> vmap;
    BOOST_CHECK_EQUAL(vmap(t1), *t1);
    delete t1;
}

BOOST_AUTO_TEST_CASE(default_ctor) {
    typedef vector<int*> container_type;
    typedef container_type::iterator iterator;
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type> > vm_iterator;

    vm_iterator j;
}

BOOST_AUTO_TEST_CASE(using_dref_map_int_pointer) {
    vector<int*> v;
    typedef vector<int*>::iterator iterator;
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type > > vm_iterator;

    v.push_back(new int(1));
    v.push_back(new int(2));
    v.push_back(new int(3));

    iterator r(v.begin());
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK_EQUAL(*j, **r);

    *(vm_iterator(v.begin())) = 42;
    r = v.begin();
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK_EQUAL(*j, **r);

    for (iterator j(v.begin());  j != v.end();  ++j) delete *j;
}


BOOST_AUTO_TEST_CASE(using_dref_map_int_pair_pointer) {
    vector<pair<int,int>*> v;
    typedef vector<pair<int,int>*>::iterator iterator;
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type > > vm_iterator;

    v.push_back(new pair<int,int>(1,4));
    v.push_back(new pair<int,int>(2,5));
    v.push_back(new pair<int,int>(3,6));

    iterator r(v.begin());
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK(*j == **r);

    (vm_iterator(v.begin()))->first = 42;
    r = v.begin();
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK(*j == **r);

    for (iterator j(v.begin());  j != v.end();  ++j) delete *j;
}

BOOST_AUTO_TEST_SUITE_END()
