#include "AssetImporter.h"
#include "MathTypes.h"
#include "Logging.h"

static void ProcessMesh(aiMesh* mesh, [[maybe_unused]] const aiScene* scene)
{
	const unsigned int numVertices = mesh->mNumVertices;
	const unsigned int numFaces = mesh->mNumFaces;

	// Mesh definition. TODO: Return these arrays somehow.
	std::vector<LeviathanCore::MathTypes::Vector3> positions(static_cast<size_t>(numVertices));
	std::vector<LeviathanCore::MathTypes::Vector3> normals(static_cast<size_t>(numVertices));
	std::vector<LeviathanCore::MathTypes::Vector2> textureCoordinates(static_cast<size_t>(numVertices));
	std::vector<uint32_t> indices(static_cast<size_t>(3 * numFaces));

	// Process mesh vertices.
	for (size_t i = 0; i < static_cast<size_t>(numVertices); ++i)
	{
		const aiVector3D& position = mesh->mVertices[i];
		positions[i] = LeviathanCore::MathTypes::Vector3(position.x, position.y, position.z);

		const aiVector3D& normal = mesh->mNormals[i];
		normals[i] = LeviathanCore::MathTypes::Vector3(normal.x, normal.y, normal.z);

		// Check if the mesh has a uv set at index 0.
		// TODO: Only supporting a single uv set (the one at index 0). Support multiple texture uv sets.
		if (mesh->mTextureCoords[0])
		{
			const aiVector3D& textureCoordinate = mesh->mTextureCoords[0][i];
			textureCoordinates[i] = LeviathanCore::MathTypes::Vector2(textureCoordinate.x, textureCoordinate.y);
		}
	}

	// Process mesh indices.
	for (size_t i = 0, j = 0; i < numFaces; ++i, j += 3)
	{
		const aiFace& face = mesh->mFaces[i];

		for (size_t k = 0; k < face.mNumIndices; ++k)
		{
			indices[j + k] = face.mIndices[k];
		}
	}

	// TODO: Process material.
	// TODO: Process skeleton bones.
}

static void ProcessNode(aiNode* node, const aiScene* scene)
{
	// Process each mesh in the node.
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene);
	}

	// Process all child nodes.
	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

bool AssetImporter::Initialize()
{
	return true;
}

bool AssetImporter::Shutdown()
{
	return true;
}

bool AssetImporter::LoadModel(const std::string& filepath)
{
	// TODO: Move model related code into separate model importer file.
	Assimp::Importer importer = {};
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if ((!scene) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
	{
		LEVIATHAN_LOG("Failed to import model. %s", importer.GetErrorString());
		return false;
	}

	ProcessNode(scene->mRootNode, scene);

	return true;
}
