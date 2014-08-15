#pragma once

namespace intergdb { namespace common
{
  class SystemConstants
  {

  public:

      static const int edgeIdSize = 8;
      
      static const int timestampSize = 8;
      
      static const int headVertexSize = 8;
      
      static const int numberOfEdgesInANeighborList = 1; // TODO

      static const int numberOfNeighborLists = 1; // TODO
      
      SystemConstants() = default;
  };
} }
