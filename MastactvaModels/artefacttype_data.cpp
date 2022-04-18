/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "artefacttype_data.h"
#include "../MastactvaBase/utils.h"


QString getArtefactFileFilter(int artefactTypeId_)
{
    const ArtefactTypeEn artefactType = to_enum<ArtefactTypeEn>(artefactTypeId_);

    static const char * vertexFilter = "Shader files (*.vert *.vertex *vsh)";
    static const char * fragmentFilter = "Shader files (*.frag *.fragment *.fsh)";
    static const char * textureFilter = "Texture files (*.png *.jpg *.jpeg)";
    static const char * jsonFilter = "Json data files (*.json)";
    static const char * obj3dFilter = "3D Objext files (*.obj)";
    static const char * luaFilter = "Lua script file (*.lua)";
    static const char * anyFilter = "Any file (*.*)";
    static const std::pair<ArtefactTypeEn, const char *> filters[] =
    {
        { ArtefactTypeEn::shaderVertex, vertexFilter},
        { ArtefactTypeEn::shaderFragmet, fragmentFilter},
        { ArtefactTypeEn::texture1D, textureFilter},
        { ArtefactTypeEn::texture2D, textureFilter},
        { ArtefactTypeEn::texture3D, textureFilter},
        { ArtefactTypeEn::dataJson, jsonFilter},
        { ArtefactTypeEn::dataObj3D, obj3dFilter},
        { ArtefactTypeEn::convertNamesJson, jsonFilter},
        { ArtefactTypeEn::scriptLua, luaFilter},
        { ArtefactTypeEn::scriptLuaRuntime, luaFilter},
        { ArtefactTypeEn::scriptLibrary, luaFilter},
        { ArtefactTypeEn::argumentReference, anyFilter},
    };
    const auto fit = std::find_if(
                std::begin(filters),
                std::end(filters),
                [&artefactType](const std::pair<ArtefactTypeEn, const char *> &v_)->bool
    {
        return v_.first == artefactType;
    });
    return std::end(filters) != fit ? fit->second : anyFilter;
}

QStringList getArtefactFileExtensions(int artefactTypeId_)
{
    const ArtefactTypeEn artefactType = to_enum<ArtefactTypeEn>(artefactTypeId_);

    static std::initializer_list<const char *> vertexFilter = {".vert", ".vertex", "*vsh"};
    static std::initializer_list<const char *> fragmentFilter = {".frag", ".fragment", ".fsh"};
    static std::initializer_list<const char *> textureFilter = {".png", ".jpg", ".jpeg"};
    static std::initializer_list<const char *> jsonFilter = {".json",};
    static std::initializer_list<const char *> obj3dFilter = {".obj",};
    static std::initializer_list<const char *> luaFilter = {".lua",};
    static std::initializer_list<const char *> anyFilter = {".*",};
    static const std::pair<ArtefactTypeEn, QStringList> filters[] =
    {
        { ArtefactTypeEn::shaderVertex, QStringList(std::begin(vertexFilter), std::end(vertexFilter))},
        { ArtefactTypeEn::shaderFragmet, QStringList(std::begin(fragmentFilter), std::end(fragmentFilter))},
        { ArtefactTypeEn::texture1D, QStringList(std::begin(textureFilter), std::end(textureFilter))},
        { ArtefactTypeEn::texture2D, QStringList(std::begin(textureFilter), std::end(textureFilter))},
        { ArtefactTypeEn::texture3D, QStringList(std::begin(textureFilter), std::end(textureFilter))},
        { ArtefactTypeEn::dataJson, QStringList(std::begin(jsonFilter), std::end(jsonFilter))},
        { ArtefactTypeEn::dataObj3D,QStringList(std::begin(obj3dFilter), std::end(obj3dFilter))},
        { ArtefactTypeEn::convertNamesJson, QStringList(std::begin(jsonFilter), std::end(jsonFilter))},
        { ArtefactTypeEn::scriptLua, QStringList(std::begin(luaFilter), std::end(luaFilter))},
        { ArtefactTypeEn::scriptLuaRuntime, QStringList(std::begin(luaFilter), std::end(luaFilter))},
        { ArtefactTypeEn::scriptLibrary, QStringList(std::begin(luaFilter), std::end(luaFilter))},
        { ArtefactTypeEn::argumentReference, QStringList(std::begin(anyFilter), std::end(anyFilter))},
    };
    const auto fit = std::find_if(
                std::begin(filters),
                std::end(filters),
                [&artefactType](const std::pair<ArtefactTypeEn, QStringList> &v_)->bool
    {
        return v_.first == artefactType;
    });
    return std::end(filters) != fit ? fit->second : QStringList(std::begin(anyFilter), std::end(anyFilter));
}
