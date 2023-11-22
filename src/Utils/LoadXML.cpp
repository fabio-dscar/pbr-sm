#include <LoadXML.h>

#include <PBRMath.h>

#include <sstream>

using namespace pbr;
using namespace pbr::math;

LoadXML::LoadXML(const std::string& filename) {
    _result = _doc.load_file(filename.c_str());
    processXML();
}

void LoadXML::processXML() {
    pugi::xml_node root = _doc.document_element();
    populateFloatMap(root);
    populateRgbMap(root);
    populateTextureMap(root);
    populateInfoMap(root);
};

const ParameterMap& LoadXML::getMap() const {
    return _map;
}

void LoadXML::populateFloatMap(const pugi::xml_node root) {
    for (pugi::xml_node floatElem : root.children("float")) {
        std::string name = floatElem.attribute("name").as_string();
        float value = floatElem.attribute("value").as_float();
        _map.setFloat(name, value);
    }
};

void LoadXML::populateRgbMap(const pugi::xml_node root) {
    for (pugi::xml_node rgbElems : root.children("rgb")) {
        std::string name = rgbElems.attribute("name").as_string();
        std::string value = rgbElems.attribute("value").as_string();

        Color rgb;
        std::stringstream ss(value);
        ss >> rgb;

        _map.setRGB(name, rgb);
    }
};

void LoadXML::populateTextureMap(const pugi::xml_node root) {
    for (pugi::xml_node textElems : root.children("texture")) {
        std::string name = textElems.attribute("name").as_string();
        std::string value = textElems.attribute("value").as_string();
        _map.setTexture(name, value);
    }
};

void LoadXML::populateInfoMap(const pugi::xml_node root) {
    std::string name = root.attribute("id").as_string();
    std::string value = root.attribute("type").as_string();
    _map.setMatInfo(name, value);
};