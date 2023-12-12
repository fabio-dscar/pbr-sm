#ifndef __PBR_RESOURCES_H__
#define __PBR_RESOURCES_H__

#include <unordered_map>

#include <PBR.h>

// Macro to syntax sugar the singleton getter
// ex: Resource.getTexture("albedo");
#define Resource Resources::get()

namespace pbr {

    class Shape;
    class Geometry;
    class Program;
    class Texture;

    template<class KT, class T> 
    using map = std::unordered_map<KT, T>;

    class Resources {
    public:
        static Resources& get();

        void initialize();

        void addGeometry(const std::string& name, const sref<Geometry>& geometry);
        void addShape   (const std::string& name, const sref<Shape>& shape);
        void addShader  (const std::string& name, const sref<Program>& shader);
        void addTexture (const std::string& name, const sref<Texture>& texture);

        bool deleteGeometry(const std::string& name);
        bool deleteShape   (const std::string& name);
        bool deleteShader  (const std::string& name);
        bool deleteTexture (const std::string& name);

        Geometry* getGeometry(const std::string& name);
        Shape*    getShape   (const std::string& name);
        Program*  getShader  (const std::string& name);
        Texture*  getTexture (const std::string& name);

        void cleanup();

    private:
        Resources() = default;

        map<std::string, sref<Geometry>> _geometry;
        map<std::string, sref<Shape>>    _shapes;
        map<std::string, sref<Program>>  _shaders;
        map<std::string, sref<Texture>>  _textures;
    };

}

#endif