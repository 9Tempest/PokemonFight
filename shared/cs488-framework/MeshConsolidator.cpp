#include "MeshConsolidator.hpp"
using namespace glm;
using namespace std;

#include "cs488-framework/Exception.hpp"
#include "cs488-framework/ObjFileDecoder.hpp"
#include <iostream>

//----------------------------------------------------------------------------------------
// Default constructor
MeshConsolidator::MeshConsolidator()
{

}

//----------------------------------------------------------------------------------------
// Destructor
MeshConsolidator::~MeshConsolidator()
{

}

//----------------------------------------------------------------------------------------
template <typename T>
static void appendVector (
		std::vector<T> & dest,
		const std::vector<T> & source
) {
	// Increase capacity to hold source.size() more elements
	dest.reserve(dest.size() + source.size());

	dest.insert(dest.end(), source.begin(), source.end());
}


//----------------------------------------------------------------------------------------
MeshConsolidator::MeshConsolidator(
		std::initializer_list<ObjFilePath> objFileList
) {

	MeshId meshId;
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> uvCoords;
	BatchInfo batchInfo;
	unsigned long indexOffset(0);

    for(const ObjFilePath & objFile : objFileList) {
	    ObjFileDecoder::decode(objFile.c_str(), meshId, positions, normals, uvCoords);

	    uint numIndices = positions.size();

	    if (numIndices != normals.size()) {
			std::cerr << "Error within MeshConsolidator: positions.size() != uvs.size()" << "filling 0,0 for all UVs!" << std::endl;
		    throw Exception("Error within MeshConsolidator: "
					"positions.size() != normals.size()\n");
	    }

		if (numIndices != uvCoords.size()){
			// Fill 0,0 for all UVs
			uvCoords.reserve(numIndices);
			for (unsigned int i = 0; i < numIndices; ++i) {
				uvCoords.emplace_back(std::move(glm::vec2(0.0f, 0.0f)));
			}
		}

		if (numIndices != uvCoords.size()){
			throw Exception("Error within MeshConsolidator: "
					"uvCoords.size() != positions.size()\n");
		}


	    batchInfo.startIndex = indexOffset;
	    batchInfo.numIndices = numIndices;

	    m_batchInfoMap[meshId] = batchInfo;

	    appendVector(m_vertexPositionData, positions);
	    appendVector(m_vertexNormalData, normals);
		appendVector(m_vertexUVData, uvCoords);

	    indexOffset += numIndices;

		assert(numIndices % 3 == 0); // must be divisible by 3
    }

}

//----------------------------------------------------------------------------------------
void MeshConsolidator::getBatchInfoMap (
		BatchInfoMap & batchInfoMap
) const {
	batchInfoMap = m_batchInfoMap;
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex position data.
const float * MeshConsolidator::getVertexPositionDataPtr() const {
	return &(m_vertexPositionData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the starting memory location for vertex normal data.
const float * MeshConsolidator::getVertexNormalDataPtr() const {
    return &(m_vertexNormalData[0].x);
}

const float * MeshConsolidator::getVertexUVDataPtr() const {
	return &(m_vertexUVData[0].x);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex position data.
size_t MeshConsolidator::getNumVertexPositionBytes() const {
	return m_vertexPositionData.size() * sizeof(vec3);
}

//----------------------------------------------------------------------------------------
// Returns the total number of bytes of all vertex normal data.
size_t MeshConsolidator::getNumVertexNormalBytes() const {
	return m_vertexNormalData.size() * sizeof(vec3);
}

size_t MeshConsolidator::getNumVertexUVBytes() const{
	return m_vertexUVData.size() * sizeof(vec2);
}
