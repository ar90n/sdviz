# include <boost/uuid/uuid.hpp>
# include <boost/uuid/uuid_io.hpp>
# include <boost/uuid/uuid_generators.hpp>
# include <boost/lexical_cast.hpp>

# include "resource.hpp"

#ifdef DEBUG_USE_EXTERNAL_TEMPLATE
# include <fstream>
# include <sstream>
# include <stdexcept>
#endif

#ifdef DEBUG_EXTERNAL_MAIN_PAGE_TEMPLATE_PATH
# define DEBUG_EXTERNAL_MAIN_PAGE_TEMPLATE_PATH "./index.html"
#endif

using namespace sdviz;

namespace
{
#ifdef DEBUG_USE_EXTERNAL_TEMPLATE
    std::string const readLocalFile( std::string const& _file_path )
    {
        std::ifstream ifs( _file_path, std::ios::in );
        if( ifs.fail() )
        {
            std::stringstream ss;
            ss << _file_path << " is not found. ";
            return ss.str();
        }

        std::string line;
        std::stringstream ss;
        while( std::getline( ifs, line ) )
        {
            ss << line << std::endl;
        }

        return ss.str();
    }
#else
    #include "../resource/dist/index_bundle.html.h"
#endif

}

std::string const sdviz::getMainPage()
{
#ifdef DEBUG_USE_EXTERNAL_TEMPLATE
    std::string const main_page = readLocalFile( DEBUG_EXTERNAL_MAIN_PAGE_TEMPLATE_PATH );
#else
    static std::string const main_page( __dist_index_bundle_html, __dist_index_bundle_html + __dist_index_bundle_html_len );
#endif

    return main_page;
}

std::string sdviz::generateElmenetId()
{
    return boost::lexical_cast< std::string >( boost::uuids::random_generator()() );
}

bool sdviz::isValidElementId( std::string const& _id )
{
    bool const isValidElementId = ( 0 < element_store.count( _id ) );

    return isValidElementId;
}

std::string sdviz::dummy_id{ "" };
std::string sdviz::page_id{ "__page__" };

ElementStore::value_type initial_elements[] = { std::make_pair( page_id,
                                                                ContainerElementImpl{ LayoutImpl{},
                                                                                      ContainerElementImplParam{ "", true } } ) };
ElementStore sdviz::element_store{ std::make_move_iterator( std::begin( initial_elements ) ),
                                   std::make_move_iterator( std::end( initial_elements ) ) };
