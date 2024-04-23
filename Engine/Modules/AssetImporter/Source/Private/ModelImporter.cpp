#include "ModelImporter.h"
#include "AssetTypes.h"
#include "MathTypes.h"
#include "Logging.h"

// Mesh attributes are appended onto the end of the vectors passed to out parameters.
static AssetImporter::AssetTypes::Mesh ProcessMesh(aiMesh* mesh, [[maybe_unused]] const aiScene* scene)
{
	const unsigned int numVertices = mesh->mNumVertices;
	const unsigned int numFaces = mesh->mNumFaces;

	AssetImporter::AssetTypes::Mesh result = {};

	// Pre-allocate memory for mesh attribute data.
	result.Positions.reserve(static_cast<size_t>(numVertices));
	result.Normals.reserve(static_cast<size_t>(numVertices));
	result.TextureCoordinates.reserve(static_cast<size_t>(numVertices));
	result.Indices.reserve(static_cast<size_t>(3 * numFaces));

	// Process mesh vertices.
	for (size_t i = 0; i < static_cast<size_t>(numVertices); ++i)
	{
		const aiVector3D& position = mesh->mVertices[i];
		result.Positions.emplace_back(position.x, position.y, position.z);

		const aiVector3D& normal = mesh->mNormals[i];
		result.Normals.emplace_back(normal.x, normal.y, normal.z);

		// Check if the mesh has a uv set at index 0.
		// TODO: Only supporting a single uv set (the one at index 0). Support multiple texture uv sets.
		if (mesh->mTextureCoords[0])
		{
			const aiVector3D& textureCoordinate = mesh->mTextureCoords[0][i];
			result.TextureCoordinates.emplace_back(textureCoordinate.x, textureCoordinate.y);
		}
	}

	// Process mesh indices.
	for (size_t i = 0, j = 0; i < numFaces; ++i, j += 3)
	{
		const aiFace& face = mesh->mFaces[i];

		for (size_t k = 0; k < face.mNumIndices; ++k)
		{
			result.Indices.push_back(face.mIndices[k]);
		}
	}

	// TODO: Process materials.
	// TODO: Process bones.

	return result;
}

// Processes each mesh in the node being processed. Each meshe's attribute data is appended to the end of the out vectors.
static void ProcessNode(aiNode* node, const aiScene* scene, AssetImporter::AssetTypes::Model& outModel)
{
	// Process each mesh in the node.
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		outModel.Meshes.emplace_back(ProcessMesh(mesh, scene));
	}

	// Process all child nodes.
	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene, outModel);
	}
}

bool AssetImporter::ModelImporter::LoadModel(std::string_view filepath, AssetImporter::AssetTypes::Model& outModel)
{
	Assimp::Importer importer = {};
	const aiScene* scene = importer.ReadFile(filepath.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if ((!scene) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
	{
		LEVIATHAN_LOG("Failed to import model. %s", importer.GetErrorString());
		return false;
	}

	ProcessNode(scene->mRootNode, scene, outModel);

	return true;
}
