#include "ModelImporter.h"
#include "AssetTypes.h"
#include "MathTypes.h"
#include "MathLibrary.h"
#include "Logging.h"

// Returns the tangent vector for the triangle.
static LeviathanCore::MathTypes::Vector3 CalculateTangent(const LeviathanCore::MathTypes::Vector3& Position0,
	const LeviathanCore::MathTypes::Vector3& Position1,
	const LeviathanCore::MathTypes::Vector3& Position2,
	const LeviathanCore::MathTypes::Vector2& TextureCoordinate0,
	const LeviathanCore::MathTypes::Vector2& TextureCoordinate1,
	const LeviathanCore::MathTypes::Vector2& TextureCoordinate2)
{
	// Calculate tangent vector. Bitangent is calculated in the shader by taking the cross product between the tangent and the surface normal.
	const LeviathanCore::MathTypes::Vector3 edge1 = Position1 - Position0;
	const LeviathanCore::MathTypes::Vector3 edge2 = Position2 - Position0;
	const LeviathanCore::MathTypes::Vector2 deltaUV1 = TextureCoordinate1 - TextureCoordinate0;
	const LeviathanCore::MathTypes::Vector2 deltaUV2 = TextureCoordinate2 - TextureCoordinate0;

	const float f = 1.0f / (deltaUV1.X() * deltaUV2.Y() - deltaUV2.X() * deltaUV1.Y());
	return LeviathanCore::MathTypes::Vector3
	(
		f * (deltaUV2.Y() * edge1.X() - deltaUV1.Y() * edge2.X()),
		f * (deltaUV2.Y() * edge1.Y() - deltaUV1.Y() * edge2.Y()),
		f * (deltaUV2.Y() * edge1.Z() - deltaUV1.Y() * edge2.Z())
	);
}

static std::vector<LeviathanCore::MathTypes::Vector3> BuildTangentsList(const size_t vertexCount, const LeviathanCore::MathTypes::Vector3* pPositions, 
	const LeviathanCore::MathTypes::Vector2* pTextureCoordinates, const size_t indexCount, const uint32_t* pIndices)
{
	std::vector<LeviathanCore::MathTypes::Vector3> tangents(vertexCount, {});
	for (size_t i = 0; i < indexCount; i += 3)
	{
		const LeviathanCore::MathTypes::Vector3 tangent = CalculateTangent(pPositions[pIndices[i]],
			pPositions[pIndices[i + 1]],
			pPositions[pIndices[i + 2]],
			pTextureCoordinates[pIndices[i]],
			pTextureCoordinates[pIndices[i + 1]],
			pTextureCoordinates[pIndices[i + 2]]);

		tangents[pIndices[i]] = tangent;
		tangents[pIndices[i + 1]] = tangent;
		tangents[pIndices[i + 2]] = tangent;
	}
	return tangents;
}

// Mesh attributes are appended onto the end of the vectors passed to out parameters.
static LeviathanAssets::AssetTypes::Mesh ProcessMesh(aiMesh* mesh, [[maybe_unused]] const aiScene* scene)
{
	const unsigned int numVertices = mesh->mNumVertices;
	const unsigned int numFaces = mesh->mNumFaces;

	LeviathanAssets::AssetTypes::Mesh result = {};

	// Pre-allocate memory for mesh attribute data.
	result.Positions.reserve(static_cast<size_t>(numVertices));
	result.Normals.reserve(static_cast<size_t>(numVertices));
	result.TextureCoordinates.reserve(static_cast<size_t>(numVertices));
	result.Tangents.reserve(static_cast<size_t>(numVertices));
	result.Indices.reserve(static_cast<size_t>(3 * numFaces));

	// Process mesh vertices.
	for (size_t i = 0; i < static_cast<size_t>(numVertices); ++i)
	{
		// Positions.
		const aiVector3D& position = mesh->mVertices[i];
		result.Positions.emplace_back(position.x, position.y, position.z);

		// Normals.
		const aiVector3D& normal = mesh->mNormals[i];
		result.Normals.emplace_back(normal.x, normal.y, normal.z);

		// Texture coordinates.
		// Check if the mesh has a uv set at index 0.
		// TODO: Only supporting a single uv set (the one at index 0). Support multiple texture uv sets.
		if (mesh->mTextureCoords[0])
		{
			const aiVector3D& textureCoordinate = mesh->mTextureCoords[0][i];
			result.TextureCoordinates.emplace_back(textureCoordinate.x, textureCoordinate.y);
		}

		// Tangents.
		const aiVector3D& tangent = mesh->mTangents[i];
		result.Tangents.emplace_back(tangent.x, tangent.y, tangent.z);
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
static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<LeviathanAssets::AssetTypes::Mesh>& outMeshes)
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

bool LeviathanAssets::ModelImporter::LoadModel(std::string_view file, std::vector<AssetTypes::Mesh>& outMeshes)
{
	outMeshes.clear();

	Assimp::Importer importer = {};
	const aiScene* scene = importer.ReadFile(file.data(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

	if ((!scene) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
	{
		LEVIATHAN_LOG("Failed to import model. %s", importer.GetErrorString());
		return false;
	}

	ProcessNode(scene->mRootNode, scene, outMeshes);

	return true;
}

LeviathanAssets::AssetTypes::Mesh LeviathanAssets::ModelImporter::CombineMeshes(const AssetTypes::Mesh* const meshes, const size_t count)
{
	if ((!meshes) || (count == 0))
	{
		return AssetTypes::Mesh();
	}

	AssetTypes::Mesh result = {};

	// Count total number of vertices and indices in the model.
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
	result.Tangents.reserve(vertexCount);
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
			result.Tangents.push_back(meshes[i].Tangents[j]);
		}

		for (size_t j = 0; j < meshes[i].Indices.size(); ++j)
		{
			// For each index in the mesh.

			// Offset indices into the vertex buffer as vertices from multiple meshes are being combined.
			result.Indices.push_back(static_cast<unsigned int>(indexOffset) + meshes[i].Indices[j]);
		}
	}

	return result;
}

LeviathanAssets::AssetTypes::Mesh LeviathanAssets::ModelImporter::GeneratePlanePrimitive(float halfWidth)
{
	AssetTypes::Mesh result =
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
			LeviathanCore::MathTypes::Vector2(0.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 1.0f),
			LeviathanCore::MathTypes::Vector2(1.0f, 0.0f),
			LeviathanCore::MathTypes::Vector2(0.0f, 0.0f)
		},

		.Indices =
		{
			0, 1, 2, 2, 3, 0
		}
	};

	// Calculate tangents
	result.Tangents = BuildTangentsList(result.Positions.size(), result.Positions.data(), result.TextureCoordinates.data(), result.Indices.size(), result.Indices.data());
	return result;
}

LeviathanAssets::AssetTypes::Mesh LeviathanAssets::ModelImporter::GenerateCubePrimitive(float halfWidth)
{
	AssetTypes::Mesh result =
	{
		.Positions =
		{
			LeviathanCore::MathTypes::Vector3{halfWidth, halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, -halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, -halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, -halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, -halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, -halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, -halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, -halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, -halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, -halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, -halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, -halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, -halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, halfWidth, -halfWidth},
			LeviathanCore::MathTypes::Vector3{-halfWidth, halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, halfWidth, halfWidth},
			LeviathanCore::MathTypes::Vector3{halfWidth, halfWidth, -halfWidth}
		},

		.Normals =
		{
			LeviathanCore::MathTypes::Vector3{0.000000f, 0.000000f, 1.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 0.000000f, 1.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 0.000000f, 1.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 0.000000f, 1.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, -1.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, -1.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, -1.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, -1.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{-1.000000f, 0.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{-1.000000f, 0.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{-1.000000f, 0.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{-1.000000f, 0.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 0.000000f, -1.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 0.000000f, -1.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 0.000000f, -1.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 0.000000f, -1.000000f},
			LeviathanCore::MathTypes::Vector3{1.000000f, 0.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{1.000000f, 0.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{1.000000f, 0.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{1.000000f, 0.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 1.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 1.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 1.000000f, 0.000000f},
			LeviathanCore::MathTypes::Vector3{0.000000f, 1.000000f, 0.000000f}
		},

		.TextureCoordinates =
		{
			LeviathanCore::MathTypes::Vector2{0.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 0.0f},
			LeviathanCore::MathTypes::Vector2{0.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 1.0f},
			LeviathanCore::MathTypes::Vector2{1.0f, 0.0f}
		},

		.Indices =
		{
			0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
		}
	};

	// Calculate tangents
	result.Tangents = BuildTangentsList(result.Positions.size(), result.Positions.data(), result.TextureCoordinates.data(), result.Indices.size(), result.Indices.data());
	return result;
}

LeviathanAssets::AssetTypes::Mesh LeviathanAssets::ModelImporter::GenerateSpherePrimitive(float radius, int32_t sectors, int32_t stacks)
{
	float sectorStep = 2.f * LeviathanCore::MathLibrary::Pi / static_cast<float>(sectors);
	float stackStep = LeviathanCore::MathLibrary::Pi / stacks;
	float sectorAngle = 0.f;
	float stackAngle = 0.f;

	float xy = 0.f;
	float lengthInv = 1.0f / radius;

	AssetTypes::Mesh result = {};

	// Vertices.
	for (int i = stacks; i >= 0; i--)
	{
		stackAngle = LeviathanCore::MathLibrary::Pi / 2 - i * stackStep;
		xy = radius * LeviathanCore::MathLibrary::Cos(stackAngle);

		for (int j = 0; j <= sectors; j++)
		{
			sectorAngle = j * sectorStep;

			auto const& position = result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ xy * LeviathanCore::MathLibrary::Cos(sectorAngle),
				xy * LeviathanCore::MathLibrary::Sin(sectorAngle),
				radius * LeviathanCore::MathLibrary::Sin(stackAngle) });


			static constexpr float uCoordinateScale = 1.75f;
			result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ (static_cast<float>(j) / sectors) * uCoordinateScale, // U coordinate is scale to fix texture stretching.
				-static_cast<float>(i) / stacks });

			result.Normals.emplace_back(LeviathanCore::MathTypes::Vector3{ position.X() * lengthInv,
				position.Y() * lengthInv,
				 position.Z() * lengthInv });
		}
	}

	// Indices.
	unsigned int k1 = 0;
	unsigned int k2 = 0;

	for (int i = 0; i < stacks; i++)
	{
		k1 = i * (sectors + 1);
		k2 = k1 + sectors + 1;

		for (int j = 0; j < sectors; j++, k1++, k2++)
		{
			if (i != 0)
			{
				result.Indices.push_back(k1);
				result.Indices.push_back(k2);
				result.Indices.push_back(k1 + 1);
			}

			if (i != (stacks - 1))
			{
				result.Indices.push_back(k1 + 1);
				result.Indices.push_back(k2);
				result.Indices.push_back(k2 + 1);
			}
		}
	}

	std::reverse(result.Indices.begin(), result.Indices.end());

	// Calculate tangents
	result.Tangents = BuildTangentsList(result.Positions.size(), result.Positions.data(), result.TextureCoordinates.data(), result.Indices.size(), result.Indices.data());

	// Return result.
	return result;
}

LeviathanAssets::AssetTypes::Mesh LeviathanAssets::ModelImporter::GenerateCylinderPrimitive(float baseRadius, float topRadius, float height, int32_t sectors, int32_t stacks)
{
	AssetTypes::Mesh result = {};

	// Vertices.
	// Side.
	float z = 0.f;
	float radius = 0.f;

	// Get normals for cylinder sides.
	std::vector<float> sideNormals;
	float sectorStep = LeviathanCore::MathLibrary::TwoPi / sectors;
	float sectorAngle = 0.f;

	float zAngle = LeviathanCore::MathLibrary::ATan2(baseRadius - topRadius, height);
	float x0 = LeviathanCore::MathLibrary::Cos(zAngle);
	float y0 = 0;
	float z0 = LeviathanCore::MathLibrary::Sin(zAngle);

	for (int32_t i = 0; i <= sectors; ++i)
	{
		sectorAngle = i * sectorStep;
		sideNormals.push_back(LeviathanCore::MathLibrary::Cos(sectorAngle) * x0 - LeviathanCore::MathLibrary::Sin(sectorAngle) * y0);
		sideNormals.push_back(LeviathanCore::MathLibrary::Sin(sectorAngle) * x0 + LeviathanCore::MathLibrary::Cos(sectorAngle) * y0);
		sideNormals.push_back(z0);
	}

	// Get unit circle vertices.
	std::vector<float> unitCircleVertices;

	for (int i = 0; i <= sectors; ++i)
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(LeviathanCore::MathLibrary::Cos(sectorAngle));
		unitCircleVertices.push_back(LeviathanCore::MathLibrary::Sin(sectorAngle));
		unitCircleVertices.push_back(0);
	}

	for (int i = stacks; i >= 0; i--)
	{
		z = -(height * 0.5f) + (float)i / stacks * height;
		radius = baseRadius + (float)i / stacks * (topRadius - baseRadius);
		float t = 1.0f - (float)i / stacks;

		for (int32_t j = 0, k = 0; j <= sectors; ++j, k += 3)
		{
			result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ unitCircleVertices[k] * radius, unitCircleVertices[static_cast<size_t>(k) + 1] * radius, z });
			static constexpr float uCoordinateScale = 3.5f;
			result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ static_cast<float>(j) * uCoordinateScale / sectors, t }); // U coordinate is scaled to fix texture stretching.
			result.Normals.emplace_back(sideNormals[static_cast<size_t>(k)], sideNormals[static_cast<size_t>(k) + 1], sideNormals[static_cast<size_t>(k) + 2]);
		}
	}

	int baseCapStart = (int)result.Positions.size();

	// Base cap.
	int numberOfCapVertices = sectors + 2;

	LeviathanCore::MathTypes::Vector3 capCenter{ 0.f, 0.f, -height / 2.f };

	for (int i = 0; i < numberOfCapVertices; i++)
	{
		auto const& position = result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ capCenter.X() + (baseRadius * LeviathanCore::MathLibrary::Cos((float)i * LeviathanCore::MathLibrary::TwoPi / (float)sectors)),
			capCenter.Y() + (baseRadius * LeviathanCore::MathLibrary::Sin((float)i * LeviathanCore::MathLibrary::TwoPi / (float)sectors)),
			capCenter.Z() });

		result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ 0.5f - (position.X()) / (2.f * baseRadius), 0.5f - (position.Y()) / (2.f * baseRadius) });

		result.Normals.emplace_back(LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, -1.0f });
	}

	{
		result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ capCenter.X(), capCenter.Y(), capCenter.Z() });

		result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ 0.5f, 0.5f });

		result.Normals.emplace_back(LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, -1.0f });
	}

	int topCapStart = (int)result.Positions.size();

	// Top cap.
	capCenter = LeviathanCore::MathTypes::Vector3{ 0.f, 0.f, height / 2.f };

	for (int i = numberOfCapVertices; i > 0; i--)
	{
		auto const& position = result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ capCenter.X() + (topRadius * LeviathanCore::MathLibrary::Cos((float)i * LeviathanCore::MathLibrary::TwoPi / (float)sectors)),
			capCenter.Y() + (topRadius * LeviathanCore::MathLibrary::Sin((float)i * LeviathanCore::MathLibrary::TwoPi / (float)sectors)),
			capCenter.Z() });

		result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ 0.5f + (position.X()) / (2.f * topRadius), 0.5f + (position.Y()) / (2.f * topRadius) });

		result.Normals.emplace_back(LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, 1.0f });
	}

	{
		result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ capCenter.X(), capCenter.Y(), capCenter.Z() });

		result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ 0.5f, 0.5f });

		result.Normals.emplace_back(LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, 1.0f });
	}

	// Indices.
	// Base cap.
	for (int32_t i = baseCapStart + 2; i < baseCapStart + sectors; i++)
	{
		result.Indices.push_back(baseCapStart);
		result.Indices.push_back(i - 1);
		result.Indices.push_back(i);
	}

	// Top cap.
	for (int32_t i = topCapStart + 2; i < topCapStart + sectors; i++)
	{
		result.Indices.push_back(topCapStart);
		result.Indices.push_back(i - 1);
		result.Indices.push_back(i);
	}

	// Side.
	unsigned int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectors + 1);
		k2 = k1 + sectors + 1;

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			result.Indices.push_back(k1);
			result.Indices.push_back(k1 + 1);
			result.Indices.push_back(k2);
			result.Indices.push_back(k2);
			result.Indices.push_back(k1 + 1);
			result.Indices.push_back(k2 + 1);
		}
	}

	std::reverse(result.Indices.begin(), result.Indices.end());

	// Calculate tangents
	result.Tangents = BuildTangentsList(result.Positions.size(), result.Positions.data(), result.TextureCoordinates.data(), result.Indices.size(), result.Indices.data());

	return result;
}

LeviathanAssets::AssetTypes::Mesh LeviathanAssets::ModelImporter::GenerateConePrimitive(float baseRadius, float height, int32_t sectors, int32_t stacks)
{
	AssetTypes::Mesh result = {};

	// Vertices.
	// Side.
	float z = 0.f;
	float radius = 0.f;

	// Get normals for cylinder sides.
	std::vector<float> sideNormals;
	float sectorStep = LeviathanCore::MathLibrary::TwoPi / sectors;
	float sectorAngle = 0.f;

	float zAngle = LeviathanCore::MathLibrary::ATan2(baseRadius - 0.f, height);
	float x0 = LeviathanCore::MathLibrary::Cos(zAngle);
	float y0 = 0;
	float z0 = LeviathanCore::MathLibrary::Sin(zAngle);

	for (int i = 0; i <= sectors; ++i)
	{
		sectorAngle = i * sectorStep;
		sideNormals.push_back(LeviathanCore::MathLibrary::Cos(sectorAngle) * x0 - LeviathanCore::MathLibrary::Sin(sectorAngle) * y0);
		sideNormals.push_back(LeviathanCore::MathLibrary::Sin(sectorAngle) * x0 + LeviathanCore::MathLibrary::Cos(sectorAngle) * y0);
		sideNormals.push_back(z0);
	}

	// Get unit circle vertices.
	std::vector<float> unitCircleVertices;

	for (int i = 0; i <= sectors; ++i)
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(LeviathanCore::MathLibrary::Cos(sectorAngle));
		unitCircleVertices.push_back(LeviathanCore::MathLibrary::Sin(sectorAngle));
		unitCircleVertices.push_back(0);
	}

	for (int i = stacks; i >= 0; i--)
	{
		z = -(height * 0.5f) + (float)i / stacks * height;
		radius = baseRadius + (float)i / stacks * (0.f - baseRadius);
		float t = 1.0f - (float)i / stacks;

		for (int32_t j = 0, k = 0; j <= sectors; ++j, k += 3)
		{
			result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ unitCircleVertices[static_cast<size_t>(k)] * radius, unitCircleVertices[static_cast<size_t>(k) + 1] * radius, z });

			static constexpr float uCoordinateScale = 3.5f;
			result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ static_cast<float>(j) * uCoordinateScale / sectors, t }); // U coordinate is scaled to fix texture stretching.

			result.Normals.emplace_back(LeviathanCore::MathTypes::Vector3{ sideNormals[static_cast<size_t>(k)], sideNormals[static_cast<size_t>(k) + 1], sideNormals[static_cast<size_t>(k) + 2] });
		}
	}

	int baseCapStart = (int)result.Positions.size();

	// Base cap.
	int numberOfCapVertices = sectors + 2;

	LeviathanCore::MathTypes::Vector3 capCenter{ 0.f, 0.f, -height / 2.f };

	for (int i = 0; i < numberOfCapVertices; i++)
	{
		auto const& position = result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ capCenter.X() + (baseRadius * LeviathanCore::MathLibrary::Cos((float)i * LeviathanCore::MathLibrary::TwoPi / (float)sectors)),
			capCenter.Y() + (baseRadius * LeviathanCore::MathLibrary::Sin((float)i * LeviathanCore::MathLibrary::TwoPi / (float)sectors)),
			capCenter.Z() });

		result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ 0.5f - (position.X()) / (2.f * baseRadius), 0.5f - (position.Y()) / (2.f * baseRadius) });

		result.Normals.emplace_back(LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, -1.0f });
	}

	{
		result.Positions.emplace_back(LeviathanCore::MathTypes::Vector3{ capCenter.X(), capCenter.Y(), capCenter.Z() });

		result.TextureCoordinates.emplace_back(LeviathanCore::MathTypes::Vector2{ 0.5f, 0.5f });

		result.Normals.emplace_back(LeviathanCore::MathTypes::Vector3{ 0.0f, 0.0f, -1.0f });
	}

	// Indices.
	// Base cap.
	for (int32_t i = baseCapStart + 2; i < baseCapStart + sectors; i++)
	{
		result.Indices.push_back(baseCapStart);
		result.Indices.push_back(i - 1);
		result.Indices.push_back(i);
	}

	// Side.
	unsigned int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (sectors + 1);
		k2 = k1 + sectors + 1;

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			result.Indices.push_back(k1);
			result.Indices.push_back(k1 + 1);
			result.Indices.push_back(k2);
			result.Indices.push_back(k2);
			result.Indices.push_back(k1 + 1);
			result.Indices.push_back(k2 + 1);
		}
	}

	std::reverse(result.Indices.begin(), result.Indices.end());

	// Calculate tangents
	result.Tangents = BuildTangentsList(result.Positions.size(), result.Positions.data(), result.TextureCoordinates.data(), result.Indices.size(), result.Indices.data());

	return result;
}
