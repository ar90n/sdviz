#ifndef __SDVIZ_RESOURCE_HPP__
# define __SDVIZ_RESOURCE_HPP__

# include <string>
# include <unordered_map>

# include <boost/variant.hpp>

# include "element_impl.hpp"

namespace sdviz
{
    using ElementStore = std::unordered_map< std::string, ElementImplVariant >;

    std::string const getMainPage();
    std::string generateElmenetId();
    bool isValidElementId( std::string const& _id );

    extern std::string dummy_id;
    extern std::string page_id;
    extern ElementStore element_store;
}

#endif // __SDVIZ_RESOURCE_HPP__
