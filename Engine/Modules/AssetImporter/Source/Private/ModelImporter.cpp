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
static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<AssetImporter::AssetTypes::Mesh>& outMeshes)
{
	// Process each mesh in the node.
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		outMeshes.emplace_back(ProcessMesh(mesh, scene));
	}

	// Process all child nodes.
	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene, outMeshes);
	}
}

bool AssetImporter::ModelImporter::LoadModel(std::string_view file, std::vector<AssetImporter::AssetTypes::Mesh>& outMeshes)
{
	Assimp::Importer importer = {};
	const aiScene* scene = importer.ReadFile(file.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if ((!scene) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
	{
		LEVIATHAN_LOG("Failed to import model. %s", importer.GetErrorString());
		return false;
	}

	ProcessNode(scene->mRootNode, scene, outMeshes);

	LEVIATHAN_LOG("Imported model: %s", file.data());

	return true;
}

AssetImporter::AssetTypes::Mesh AssetImporter::ModelImporter::CombineMeshes(const AssetImporter::AssetTypes::Mesh* const meshes, const size_t count)
{
	if ((!meshes) || (count == 0))
	{
		return AssetImporter::AssetTypes::Mesh();
	}

	AssetImporter::AssetTypes::Mesh result = {};

	// Count total number of vertices and indices in the model and .
	size_t vertexCount = 0;
	size_t indexCount = 0;
	for (size_t i = 0; i < count; ++i)
	{
		vertexCount += meshes[i].Positions.size();
		indexCount += meshes[i].Indices.size();
	}

	// Allocate space to store the combined mesh.
	result.Positions.reserve(vertexCount);
	result.Normals.reserve(vertexCount);
	result.TextureCoordinates.reserve(vertexCount);
	result.Indices.reserve(indexCount);

	// Build combined mesh.
	for (size_t i = 0; i < count; ++i)
	{
		// For each mesh.
		size_t indexOffset = result.Positions.size();

		for (size_t j = 0; j < meshes[i].Positions.size(); ++j)
		{
			// For each vertex in the mesh.
			result.Positions.push_back(meshes[i].Positions[j]);
			result.Normals.push_back(meshes[i].Normals[j]);
			result.TextureCoordinates.push_back(meshes[i].TextureCoordinates[j]);
		}

		for (size_t j = 0; j < meshes[i].Indices.size(); ++j)
		{
			// For each index in the mesh.

			// Offset indices into the vertex buffer as vertices from multiple meshes are being combin
			result.Indices.push_back(static_cast<unsigned int>(indexOffset) + meshes[i].Indices[j]);
		}
	}

	return result;
}

AssetImporter::AssetTypes::Mesh AssetImporter::ModelImporter::GeneratePlanePrimitive(float width)
{
	const float halfWidth = width / 2.0f;

	return AssetImporter::AssetTypes::Mesh
	{
		.Positions =
		{
			LeviathanCore::MathTypes::Vector3(-halfWidth, halfWidth, 0.0f),
			LeviathanCore::MathTypes::Vector3(halfWidth, halfWidth, 0.0f),
			LeviathanCore::MathTypes::Vector3(halfWidth, -halfWidth, 0.0f),
			LeviathanCore::MathTypes::Vector3(-halfWidth, -halfWidth, 0.0f)
		},

		.Normals =
		{
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -1.0f)
		},

		.TextureCoordinates =
		{
			LeviathanCore::MathTypes::Vector2(0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 1.0f)
		},

		.Indices =
		{
			0, 1, 2, 2, 3, 0
		}
	};
}

AssetImporter::AssetTypes::Mesh AssetImporter::ModelImporter::GenerateCubePrimitive(float width)
{
	const float halfWidth = width / 2.0f;

	return AssetImporter::AssetTypes::Mesh
	{
		.Positions =
		{
			LeviathanCore::MathTypes::Vector3(-halfWidth,  -halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,   halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,   halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,  -halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,   halfWidth,  halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,  -halfWidth,  halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,  halfWidth,  halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,  -halfWidth, halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,   halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,   halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,  -halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,  -halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,  -halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,  -halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,   halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,   halfWidth, -halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,  halfWidth,  halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,  halfWidth,  halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,  -halfWidth, halfWidth),
			LeviathanCore::MathTypes::Vector3(-halfWidth,  -halfWidth, halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,   halfWidth,  halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,   halfWidth,  halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,  -halfWidth,  halfWidth),
			LeviathanCore::MathTypes::Vector3(halfWidth,  -halfWidth,  halfWidth)
		},

		.Normals =
		{
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, -1.0f),
			LeviathanCore::MathTypes::Vector3(1.0f, 0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(1.0f, 0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector3(1.0f, 0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(1.0f, 0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, -1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(-1.0f, 0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, -1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(-1.0f, 0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(-1.0f, 0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, -1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(-1.0f, 0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, 0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector3(0.0f, -1.0f, 0.0f)
		},

		.TextureCoordinates =
		{
			LeviathanCore::MathTypes::Vector2(0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 0.0f)
		},

		.Indices =
		{
			0, 1, 2, 0, 2, 3, 10, 8, 4, 10, 4, 5, 22, 20, 6, 22, 6, 7, 19, 16, 14, 19, 14, 12, 15, 17, 21, 15, 21, 9, 11, 23, 13, 23, 18, 13
		}
	};
}
