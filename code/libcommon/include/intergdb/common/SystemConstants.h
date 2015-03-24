#pragma once

namespace intergdb { namespace common
{
    class SystemConstants
    {
    public:
        static const int edgeIdSize = 8;
        static const int timestampSize = 8;
        static const int headVertexSize = 8;
        static const int numEntriesSize = 4;
        static const int numberOfEdgesInABlock = 24;
        static const int numberOfNeighborListsInABlock = 8;
        SystemConstants() = delete;
    };
} }
