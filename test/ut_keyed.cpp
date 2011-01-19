#include <boost/test/unit_test.hpp>

#include "ootree.h"
#include "ut_common.h"


BOOST_AUTO_TEST_SUITE(ut_keyed)


BOOST_AUTO_TEST_CASE(default_ctor) {
    tree<int, cscat<keyed, std::string> > t1;
    BOOST_CHECK(t1.empty());
    BOOST_CHECK_EQUAL(t1.size(), 0);
    BOOST_CHECK_EQUAL(t1.depth(), 0);
    BOOST_CHECK_THROW(t1.root(), ootree::exception);
}


BOOST_AUTO_TEST_SUITE_END()
