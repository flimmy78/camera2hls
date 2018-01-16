/**
 * Title: configure file
 * Description: parse configure file
 * Copyright: Copyright (c) 2013 Kugou (All rights reserved)
 * Company: Kugou
 * Author: zhiguangq
 * Reviser: <Please input reviser>
 * Date: 2013-10-14
 * Version: 1.0
 * History: [1.0 2013-10-14]
 */

#ifndef SYSTEMCONFIG_FACILITY_BASE_KUGOU_H
#define SYSTEMCONFIG_FACILITY_BASE_KUGOU_H

#include <istream>
#include <ostream>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <kugou/base/facility/Singleton.h>

namespace kugou
{
namespace httpproxy
{
namespace httpserver
{
namespace roommanager
{

//EXCEPTION(IOException, com::alu::base::Exception);

#define MakeType(DestType, SourType, DefaultValue) \
    struct DestType \
    { \
        DestType(SourType n = DefaultValue) \
        { \
            m_n = n; \
        } \
        operator SourType () const \
        { \
            return m_n; \
        } \
    private: \
        SourType m_n; \
    }

class SystemConfig : public kugou::base::facility::Singleton<SystemConfig>
{
    friend class kugou::base::facility::Singleton<SystemConfig>;
private:
	SystemConfig(std::istream& in);
	SystemConfig(boost::shared_ptr<std::istream> in);

	SystemConfig();
	void initConfigDesc(boost::program_options::options_description& desc);
	void measure(boost::shared_ptr<boost::program_options::option_description> od);
	void dumpConfig(const std::string& configCaption);
	void dumpParameter(boost::shared_ptr<boost::program_options::option_description> od);
	void checkMissingOption(boost::shared_ptr<boost::program_options::option_description> od);

	boost::program_options::options_description m_desc; // options description, defines how config file will be parsed.
	boost::program_options::variables_map m_vm; // map between parameter's name and variables.
	std::ostream* m_out; // Output stream for config.
	std::size_t m_w1; // space 1 to keep the output format tidy.
	std::size_t m_w2; // space 2 to keep the output format tidy.
	std::size_t m_w3; // space 3 to keep the output format tidy.
	boost::mutex m_mutexVM;
public:
	static void dumpConfig(const std::string& configCaption, std::ostream& out);

	void reload(std::istream& in);

	// Add user define type at here
	MakeType(PortInt, int, 0);
	MakeType(PositiveInt, int, 0);

	//////////////////////////////////////////////////////////////////////////////////
	/*******************User LEVEL  *********************/
	//*********system****************
    std::string m_HTTPServerIP;
	PortInt     m_HTTPPort;
};

}
}
}
}

#endif // 
