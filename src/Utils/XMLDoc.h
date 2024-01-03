#ifndef __PBR_XMLDOC_H__
#define __PBR_XMLDOC_H__

#include <pugixml/pugixml.hpp>
#include <PBR.h>
#include <optional>

namespace pbr {

class XMLElement {
public:
    XMLElement() = default;
    XMLElement(const pugi::xml_node el) : element(el) {}

    std::string name() const { return element.name(); }

    auto children() const { return element.children(); }

    template<typename T>
    T attr(const std::string& name, const T& defVal = T{}) const {
        auto xmlAttr = element.attribute(name.c_str());
        if (!xmlAttr)
            return defVal;

        std::string attrVal = xmlAttr.as_string();
        if (attrVal == "")
            return defVal;

        return readStream<T>(attrVal);
    }

    bool hasAttr(const std::string& name) const {
        return element.attribute(name.c_str());
    }

    bool hasAttrs(std::span<const std::string> attrs) const {
        for (const auto& attr : attrs)
            if (!hasAttr(attr))
                return false;
        return true;
    }

private:
    template<typename T>
    T readStream(const std::string& str) const {
        T val;
        std::stringstream ss(str);
        ss >> val;
        return val;
    }

    pugi::xml_node element;
};

template<>
inline float XMLElement::attr<float>(const std::string& name, const float& defVal) const {
    auto xmlAttr = element.attribute(name.c_str());
    return xmlAttr.as_float(defVal);
}

template<>
inline double XMLElement::attr<double>(const std::string& name,
                                       const double& defVal) const {
    auto xmlAttr = element.attribute(name.c_str());
    return xmlAttr.as_double(defVal);
}

template<>
inline std::string XMLElement::attr<std::string>(const std::string& name,
                                                 const std::string& defVal) const {
    auto xmlAttr = element.attribute(name.c_str());
    return xmlAttr.as_string(defVal.c_str());
}

class XMLDoc {
public:
    std::optional<XMLElement> open(const std::string& fileName) {
        auto result = doc.load_file(fileName.c_str());
        if (!result) {
            LOG_ERROR("Couldn't load XML file.");
            return {};
        }

        return doc.document_element();
    }

private:
    pugi::xml_document doc;
};

} // namespace pbr

#endif