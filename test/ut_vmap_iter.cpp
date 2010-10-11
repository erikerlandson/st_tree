#include <boost/test/unit_test.hpp>

#include "ootree.h"

using namespace ootree;
using namespace std;

BOOST_AUTO_TEST_SUITE(ut_valmap_iterator_adaptor)

BOOST_AUTO_TEST_CASE(dref_map) {
    int* t1 = new int(4);
    dref_vmap<typeof(t1)> vmap;
    BOOST_CHECK_EQUAL(vmap(t1), *t1);
    delete t1;
}

BOOST_AUTO_TEST_CASE(dref_map_const) {
    const int* t1 = new int(4);
    dref_vmap<typeof(t1)> vmap;
    BOOST_CHECK_EQUAL(vmap(t1), *t1);
    delete t1;
}

BOOST_AUTO_TEST_CASE(dref_map_shared_ptr) {
    shared_ptr<int> t1(new int(4));
    dref_vmap<typeof(t1)> vmap;
    BOOST_CHECK_EQUAL(vmap(t1), *t1);
}

BOOST_AUTO_TEST_CASE(dref_map_shared_ptr_const) {
    shared_ptr<int const> t1(new int(4));
    dref_vmap<typeof(t1)> vmap;
    BOOST_CHECK_EQUAL(vmap(t1), *t1);
}

BOOST_AUTO_TEST_CASE(dref_second_map_shared_ptr) {
    shared_ptr<pair<int,int> > t1(new pair<int, int>(3,4));
    dref_second_vmap<typeof(t1)> vmap;
    BOOST_CHECK_EQUAL(vmap(t1), t1->second);
}

BOOST_AUTO_TEST_CASE(dref_second_map_shared_ptr_const) {
    shared_ptr<pair<int,int> const> t1(new pair<int, int>(3,4));
    dref_second_vmap<typeof(t1)> vmap;
    BOOST_CHECK_EQUAL(vmap(t1), t1->second);
}

BOOST_AUTO_TEST_CASE(default_ctor) {
    typedef vector<int*> container_type;
    typedef container_type::iterator iterator;
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type> > vm_iterator;

    vm_iterator j;
}

BOOST_AUTO_TEST_CASE(using_dref_map_int_pointer) {
    vector<int*> v;
    typedef typeof(v.begin()) iterator;
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


BOOST_AUTO_TEST_CASE(using_dref_map_int_shared_pointer) {
    vector<shared_ptr<int> > v;
    typedef typeof(v.begin()) iterator;
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type > > vm_iterator;

    v.push_back(shared_ptr<int>(new int(1)));
    v.push_back(shared_ptr<int>(new int(2)));
    v.push_back(shared_ptr<int>(new int(3)));

    iterator r(v.begin());
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK_EQUAL(*j, **r);

    *(vm_iterator(v.begin())) = 42;
    r = v.begin();
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK_EQUAL(*j, **r);
}


BOOST_AUTO_TEST_CASE(using_dref_map_int_pair_pointer) {
    vector<pair<int,int>*> v;
    typedef typeof(v.begin()) iterator;
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


BOOST_AUTO_TEST_CASE(using_dref_map_int_pair_shared_pointer) {
    vector<shared_ptr<pair<int,int> > > v;
    typedef typeof(v.begin()) iterator;
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type > > vm_iterator;

    v.push_back(shared_ptr<pair<int,int> >(new pair<int,int>(1,4)));
    v.push_back(shared_ptr<pair<int,int> >(new pair<int,int>(2,5)));
    v.push_back(shared_ptr<pair<int,int> >(new pair<int,int>(3,6)));

    iterator r(v.begin());
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK(*j == **r);

    (vm_iterator(v.begin()))->first = 42;
    r = v.begin();
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK(*j == **r);
}


BOOST_AUTO_TEST_CASE(using_dref_second_map_int_pair_shared_pointer) {
    vector<shared_ptr<pair<int,int> > > v;
    typedef typeof(v.begin()) iterator;
    typedef valmap_iterator_adaptor<iterator, dref_second_vmap<iterator::value_type > > vm_iterator;

    v.push_back(shared_ptr<pair<int,int> >(new pair<int,int>(1,4)));
    v.push_back(shared_ptr<pair<int,int> >(new pair<int,int>(2,5)));
    v.push_back(shared_ptr<pair<int,int> >(new pair<int,int>(3,6)));

    iterator r(v.begin());
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK(*j == (*r)->second);

    *(vm_iterator(v.begin())) = 42;
    r = v.begin();
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j,++r)
        BOOST_CHECK(*j == (*r)->second);
}


BOOST_AUTO_TEST_CASE(base_iterator_cast) {
    vector<shared_ptr<int> > v;
    typedef typeof(v.begin()) iterator;
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type > > vm_iterator;

    v.push_back(shared_ptr<int>(new int(1)));

    vm_iterator j(v.begin());
    iterator r(j);
    BOOST_CHECK(r == v.begin());
    BOOST_CHECK_EQUAL(*j, **r);
}

BOOST_AUTO_TEST_SUITE_END()
