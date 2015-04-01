#pragma once

#include <intergdb/common/QueryWorkload.h>
#include <intergdb/common/SchemaStats.h>
#include <intergdb/util/ZipfRand.h>
#include <intergdb/util/NormalRand.h>
#include <intergdb/core/Query.h>
#include <intergdb/core/InteractionGraph.h>

namespace intergdb { namespace simulation
{
    class SimulationConf
    {
    public:
        SimulationConf();

        void setAttributeCount(size_t attributeCount)
        {
            attributeCount_ = attributeCount;
        }

        size_t getAttributeCount() const
        {
            return attributeCount_;
        }

        void setAttributeSizeZipfParam(double attributeSizeZipfParam)
        {
            attributeSizeZipfParam_ = attributeSizeZipfParam;
        }

        double getAttributeSizeZipfParam() const
        {
            return attributeSizeZipfParam_;
        }

        void setQueryLengthMean(size_t queryLengthMean)
        {
            queryLengthMean_ = queryLengthMean;
        }

        size_t getQueryLengthMean() const
        {
            return queryLengthMean_;
        }

        void setQueryLengthStdDev(double queryLengthStdDev)
        {
            queryLengthStdDev_ = queryLengthStdDev;
        }

        double getQueryLengthStdDev() const
        {
            return queryLengthStdDev_;
        }

        void setQueryTypeCount(size_t queryTypeCount)
        {
            queryTypeCount_ = queryTypeCount;
        }

        size_t getQueryTypeCount() const
        {
            return queryTypeCount_;
        }

        void setQueryTypeFrequencyZipfParam(
            double queryTypeFrequencyZipfParam)
        {
            queryTypeFrequencyZipfParam_ = queryTypeFrequencyZipfParam;
        }

        double getQueryTypeFrequencyZipfParam() const
        {
            return queryTypeFrequencyZipfParam_;
        }

        void setQueryTimeZipfParam(double queryTimeZipfParam)
        {
            queryTimeZipfParam_ = queryTimeZipfParam;
        }

        double getQueryTimeZipfParam() const
        {
            return queryTimeZipfParam_;
        }

        std::pair<common::QueryWorkload, common::SchemaStats>
            getQueryWorkloadAndStats(
                std::vector<std::unique_ptr<Attribute>> & attributes);

        std::vector<std::vector<std::string> > getQueryTemplates(core::InteractionGraph * graph);


    private:
        size_t attributeCount_ = 10;
        double attributeSizeZipfParam_ = 0.5;
        size_t queryLengthMean_ = 3;
        double queryLengthStdDev_ = 2.0;
        size_t queryTypeCount_ = 1;
        double queryTypeFrequencyZipfParam_ = 0.5;
        static const int numAttributeSizes_ = 7;
        static constexpr double const attributeSizes_[numAttributeSizes_] =
           {4.0, 1.0, 8.0, 2.0, 16.0, 32.0, 64.0};
        double queryTimeZipfParam_ = 0.5;
        util::ZipfRand attributeSizeGen_;
        util::NormalRand queryLengthGen_;
        util::ZipfRand queryTypeFrequencyGen_;
        util::ZipfRand queryTimeGen_;
    };
} } /* namespace */

