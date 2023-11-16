#ifndef __PBR_LXML_H__
#define __PBR_LXML_H__

#include <PBR.h>

#include <pugixml.hpp>
#include <ParameterMap.h>

namespace pbr {

class LoadXML {
public:
    LoadXML(const std::string& filename);
    void processXML();
    void populateFloatMap(const pugi::xml_node root);
    void populateRgbMap(const pugi::xml_node root);
    void populateTextureMap(const pugi::xml_node root);
    void populateInfoMap(const pugi::xml_node root);

    const ParameterMap& getMap() const;

private:
    pugi::xml_document _doc;
    pugi::xml_parse_result _result;
    ParameterMap _map;
};

} // namespace pbr

#endif