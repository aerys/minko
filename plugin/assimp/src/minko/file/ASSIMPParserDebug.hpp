#pragma once

#include <cstring>

#include "assimp/scene.h"

#include "minko/scene/Node.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/component/Animation.hpp"

static
std::ostream&
printNode(std::ostream& out, Node::Ptr node, uint depth)
{
    bool hasSurface        = node->hasComponent<Surface>();
    bool hasSkinning    = node->hasComponent<Skinning>();
    bool hasTransform    = node->hasComponent<Transform>();
    bool hasAnimation    = node->hasComponent<Animation>();
    bool hasIdentity    = false;
    if (hasTransform)
    {
        const auto& data = node->component<Transform>()->matrix();

        hasIdentity = true;
        for (unsigned int i = 0; i < 16; ++i)
            hasIdentity = hasIdentity && fabsf(data[i / 4][i % 4] - (i % 5 == 0 ? 1.0f : 0.0f)) < 1e-4f;
    }

    for (uint d = 0; d < depth; ++d)
        out << "  ";

    out << "(" << depth << ") " << node->name() << "\t";
    if (hasSurface)
        out << "[Surf] ";
    if (hasIdentity)
        out << "[Id] ";
    else if (hasTransform)
        out << "[Trf] ";
    if (hasAnimation)
        out << "[Anim] ";
    if (hasSkinning)
        out << "[Skin] ";
    out << "\n";

    for (auto& n : node->children())
        printNode(out, n, depth + 1);

    return out;
}

static
std::string
getDotLabel(const aiNode* ainode)
{
    if (ainode)
    {
        const std::string nodeName(ainode->mName.data);

        return nodeName == "unnamed" || nodeName.find("AutoName$") != std::string::npos
            ? "\"\""
            : "\"" + nodeName + "\"";
    }
    else
        return "\"\"";
}

static
std::string
getDotLabel(Node::Ptr node)
{
    if (node)
    {
        return node->name() == "unnamed" || node->name().find("AutoName$") != std::string::npos
            ? "\"\""
            : "\"" + node->name() + "\"";
    }
    else
        return "\"\"";
}

static
std::string
getDotStyle(Node::Ptr node)
{
    static const std::string whiteColor = "white";
    static const std::string blackColor = "black";
    static const std::string meshColor = "#ffe680";
    static const std::string skinColor = "#5fd35f";
    static const std::string animColor = "#ff2a2a";
    static const std::string trfColor = "#2a7fff";


    std::vector<std::string> colors;
    if (node->hasComponent<Transform>())
        colors.push_back(trfColor);
    if (node->hasComponent<Skinning>())
        colors.push_back(skinColor);
    if (node->hasComponent<Animation>())
        colors.push_back(animColor);
    if (node->hasComponent<Surface>())
        colors.push_back(meshColor);

    if (colors.size()<=1)
        return "style=filled, color=\"black\", fillcolor=\"" + (colors.empty() ? whiteColor : colors.front()) + "\"";

    std::string ret = "style=wedged, color=\"" + colors.front() + "\", fontcolor=\"black\", fillcolor=\"" + colors.front();
    for (unsigned int i = 1; i < colors.size(); ++i)
        ret += ":" + colors[i];
    ret += "\"";

    return ret;
}

static
std::string
getDotStyle(const aiScene* aiscene, const aiNode* ainode)
{
    static const std::string whiteColor = "\"white\"";
    static const std::string blackColor = "\"black\"";
    static const std::string meshColor = "\"#ffe680\"";
    static const std::string skinColor = "\"#5fd35f\"";
    static const std::string animColor = "\"#ff2a2a\"";

    static std::set<std::string>* boneNames = nullptr;
    if (boneNames == nullptr)
    {
        boneNames = new std::set<std::string>();
        for (unsigned int m = 0; m < aiscene->mNumMeshes; ++m)
            for (unsigned int b = 0; b < aiscene->mMeshes[m]->mNumBones; ++b)
            {
                boneNames->insert(std::string(aiscene->mMeshes[m]->mBones[b]->mName.data));
                std::cout << aiscene->mMeshes[m]->mBones[b]->mName.data << " is a bone" << std::endl;
            }
    }

    static std::set<std::string>* animatedNames = nullptr;
    if (animatedNames == nullptr)
    {
        animatedNames = new std::set<std::string>();
        for (unsigned int a = 0; a < aiscene->mNumAnimations; ++a)
            for (unsigned int n = 0; n < aiscene->mAnimations[a]->mNumChannels; ++n)
                animatedNames->insert(std::string(aiscene->mAnimations[a]->mChannels[n]->mNodeName.data));
    }

    const bool    isAnimated    = animatedNames->count(std::string(ainode->mName.data)) > 0;
    const bool    isBone        = boneNames->count(std::string(ainode->mName.data)) > 0;
    const bool    hasMesh        = ainode->mNumMeshes > 0;
    bool        isSkinned    = false;
    for (unsigned int m = 0; m < ainode->mNumMeshes; ++m)
        isSkinned = isSkinned || aiscene->mMeshes[ainode->mMeshes[m]]->mNumBones > 0;

    const std::string& fillColor    = hasMesh ? meshColor : (isBone ? skinColor : whiteColor);
    const std::string& color        = isAnimated ? animColor : (isSkinned ? skinColor : blackColor);
    const std::string& fontColor    = isAnimated ? animColor : blackColor;

    return "style=filled, fillcolor=" + fillColor + ", color=" + color + ", fontcolor=" + fontColor;
}

static
std::ostream&
dotPrint(std::ostream&        out,
         const aiScene*        aiscene,
         const aiNode*        ainode,
         const std::string&    nodeName)
{
    if (aiscene == nullptr || ainode == nullptr)
        return out;

    for (uint i = 0; i < ainode->mNumChildren; ++i)
    {
        const std::string& childName = nodeName + "_" + std::to_string(i);

        out << "{"
            << nodeName
            << " [label=" << getDotLabel(ainode)
            << ", " << getDotStyle(aiscene, ainode) << "]} -> {"
            << childName
            << " [label=" << getDotLabel(ainode->mChildren[i])
            << ", " << getDotStyle(aiscene, ainode->mChildren[i]) << "]}"
            << std::endl;

        dotPrint(out, aiscene, ainode->mChildren[i], childName);
    }
    return out;
}

static
std::ostream&
dotPrint(std::ostream&        out,
         Node::Ptr            node,
         const std::string&    nodeName)
{
    if (node == nullptr)
        return out;

    for (uint i = 0; i < node->children().size(); ++i)
    {
        auto child = node->children()[i];
        const std::string& childName = nodeName + "_" + std::to_string(i);

        out << "{"
            << nodeName
            << " [label=" << getDotLabel(node) << "," << getDotStyle(node) << "]} -> {"
            << childName
            << " [label=" << getDotLabel(child) << "," << getDotStyle(child) << "]}"
            << std::endl;

        dotPrint(out, child, childName);
    }
    return out;
}

static
void
dotPrint(const std::string& filename,
        const aiScene* aiscene)
{
    std::ofstream file (filename);
    if (file.is_open())
    {
        file << "digraph aiscene {\n";
        file << "node [style=filled];\n";
        dotPrint(file, aiscene, aiscene->mRootNode, "root");
        file << "}\n";
        file.close();
    }
}

static
void
dotPrint(const std::string& filename,
         Node::Ptr            scene)
{
    std::ofstream file (filename);
    if (file.is_open())
    {
        file << "digraph minkoscene {\n";
        dotPrint(file, scene, "root");
        file << "}\n";
        file.close();
    }
}