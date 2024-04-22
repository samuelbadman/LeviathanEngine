#include "ModelImporter.h"
#include "MathTypes.h"
#include "Logging.h"

// Mesh attributes are appended onto the end of the vectors passed to out parameters.
static void ProcessMesh(aiMesh* mesh,
	[[maybe_unused]] const aiScene* scene,
	std::vector<LeviathanCore::MathTypes::Vector3>& outPositions,
	std::vector<LeviathanCore::MathTypes::Vector3>& outNormals,
	std::vector<LeviathanCore::MathTypes::Vector2>& outTextureCoordinates,
	std::vector<uint32_t>& outIndices)
{
	const unsigned int numVertices = mesh->mNumVertices;
	const unsigned int numFaces = mesh->mNumFaces;

	// Pre-allocate memory for mesh attribute data.
	outPositions.reserve(outPositions.capacity() + static_cast<size_t>(numVertices));
	outNormals.reserve(outNormals.capacity() + static_cast<size_t>(numVertices));
	outTextureCoordinates.reserve(outTextureCoordinates.capacity() + static_cast<size_t>(numVertices));
	outIndices.reserve(outIndices.capacity() + static_cast<size_t>(3 * numFaces));

	// Process mesh vertices.
	for (size_t i = 0; i < static_cast<size_t>(numVertices); ++i)
	{
		const aiVector3D& position = mesh->mVertices[i];
		outPositions.emplace_back(position.x, position.y, position.z);

		const aiVector3D& normal = mesh->mNormals[i];
		outNormals.emplace_back(normal.x, normal.y, normal.z);

		// Check if the mesh has a uv set at index 0.
		// TODO: Only supporting a single uv set (the one at index 0). Support multiple texture uv sets.
		if (mesh->mTextureCoords[0])
		{
			const aiVector3D& textureCoordinate = mesh->mTextureCoords[0][i];
			outTextureCoordinates.emplace_back(textureCoordinate.x, textureCoordinate.y);
		}
	}

	// Process mesh indices.
	for (size_t i = 0, j = 0; i < numFaces; ++i, j += 3)
	{
		const aiFace& face = mesh->mFaces[i];

		for (size_t k = 0; k < face.mNumIndices; ++k)
		{
			//outIndices[j + k] = face.mIndices[k];
			outIndices.push_back(face.mIndices[k]);
		}
	}

	// TODO: Process materials.
	// TODO: Process bones.
}

// Processes each mesh in the node being processed. Each meshe's attribute data is appended to the end of the out vectors.
static void ProcessNode(aiNode* node, const aiScene* scene,
	std::vector<LeviathanCore::MathTypes::Vector3>& outPositions,
	std::vector<LeviathanCore::MathTypes::Vector3>& outNormals,
	std::vector<LeviathanCore::MathTypes::Vector2>& outTextureCoordinates,
	std::vector<uint32_t>& outIndices)
{

	// Process each mesh in the node.
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		// TODO: Here all meshes are being combined into one model. Add option to treat each mesh as its own model.

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, outPositions, outNormals, outTextureCoordinates, outIndices);
	}

	// Process all child nodes.
	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene, outPositions, outNormals, outTextureCoordinates, outIndices);
	}
}

bool AssetImporter::ModelImporter::LoadModel(std::string_view filepath,
	std::vector<LeviathanCore::MathTypes::Vector3>& outPositions,
	std::vector<LeviathanCore::MathTypes::Vector3>& outNormals,
	std::vector<LeviathanCore::MathTypes::Vector2>& outTextureCoordinates,
	std::vector<uint32_t>& outIndices)
{
	Assimp::Importer importer = {};
	const aiScene* scene = importer.ReadFile(filepath.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if ((!scene) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
	{
		LEVIATHAN_LOG("Failed to import model. %s", importer.GetErrorString());
		return false;
	}

	ProcessNode(scene->mRootNode, scene, outPositions, outNormals, outTextureCoordinates, outIndices);

	return true;
}
