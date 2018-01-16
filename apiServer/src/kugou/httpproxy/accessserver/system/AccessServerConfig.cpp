#include <kugou/httpproxy/accessserver/system/AccessServerConfig.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::program_options;

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace system
{

class ValueHelper
{
public:
    virtual const string &typeName() const = 0;
    virtual const string &defaultValue() const = 0;

    virtual ~ValueHelper() {}
};

template <typename T> class typed_value2 : public typed_value <T>, public ValueHelper
{
public:
    typed_value2(T *store_to, const string &typeName)
        : typed_value <T> (store_to)
        , m_typeName(typeName)
        , m_defaultValue("?")
    {
    }

    typed_value2 *default_value(const T &v)
    {
        m_defaultValue = boost::lexical_cast <std::string> (v);
        typed_value <T>::default_value(v);
        return this;
    }

    typed_value <T> *default_value(const T &v, const std::string &textual)
    {
        m_defaultValue = textual;
        typed_value <T>::default_value(v, textual);
        return this;
    }

    virtual const string &typeName() const
    {
        return m_typeName;
    }

    virtual const string &defaultValue() const
    {
        return m_defaultValue;
    }

private:
    string m_typeName;
    string m_defaultValue;
};

template <class T> typed_value2 <T> *value(T *v, const string &typeName)
{
    return new typed_value2 <T> (v, typeName);
}

//template <class T> typed_value2 <T> *value(const string &typeName)
//{
//    return value2 <T> (NULL, typeName);
//}

#define value2(T, v) value <T> (v, #T)

AccessServerConfig::AccessServerConfig()
    : m_desc(typeid(AccessServerConfig).name())
    , m_out(NULL)
    , m_w1(0)
    , m_w2(0)
    , m_w3(0)
{
    initConfigDesc(m_desc);

#ifdef _MSC_VER
std::string SysConfigure_File = "config\\sys.cfg";
#elif defined __GNUC__
std::string SysConfigure_File = "config/sys.cfg";
#endif
    ifstream in(SysConfigure_File.c_str());  

    if (in)
    {
        store(parse_config_file(in, m_desc), m_vm);
        notify(m_vm);
        for_each(m_desc.options().begin(), m_desc.options().end(), bind1st(mem_fun(&AccessServerConfig::checkMissingOption), this));
    }
    else
    {
        //throw IOException();
        throw 0;
    }
}

AccessServerConfig::AccessServerConfig(istream& in)
    : m_desc(typeid(AccessServerConfig).name())
    , m_out(NULL)
    , m_w1(0)
    , m_w2(0)
    , m_w3(0)
{
    initConfigDesc(m_desc);
    
    if (in)
    {
        store(parse_config_file(in, m_desc), m_vm);
        notify(m_vm);
        for_each(m_desc.options().begin(), m_desc.options().end(), bind1st(mem_fun(&AccessServerConfig::checkMissingOption), this));
    }
    else
    {
        //throw IOException();
        throw 0;
    }
}

void validate(any& v, const vector<string> & values, AccessServerConfig::PortInt*, int)
{
	try
	{
		int n = lexical_cast<int> (values[0]);
	    if (n <= 0 || 65536 <= n)
	        throw_exception(validation_error(validation_error::invalid_option_value, "value: valid port number from 1~65535", "name:"));

	    v = AccessServerConfig::PortInt(n);
	}
    catch(bad_lexical_cast& e)
    {
    	throw_exception(validation_error(validation_error::invalid_option_value, e.what(), ""));
    }
}

void validate(any& v, const vector<string> & values, AccessServerConfig::PositiveInt*, int)
{
	try
	{
		int n = lexical_cast<int> (values[0]);
		if (n < -1 )
			throw_exception(validation_error(validation_error::invalid_option_value, "value: valid number from -1,0 and positive integer", "name:"));

		v = AccessServerConfig::PositiveInt(n);
	}
	catch(bad_lexical_cast& e)
	{
		throw_exception(validation_error(validation_error::invalid_option_value, e.what(), ""));
	}

}
void AccessServerConfig::initConfigDesc(program_options::options_description& desc)
{
    desc.add_options()
	/************* Server Basic Configuration ***********/
	//system parameter
    ("SYS.CPUNumber",  value2(PortInt,     &m_CPUNumber)->default_value(1), "CPU Number")
    ("SYS.HTTPServerIP", value2(string,      &m_HTTPServerIP)->default_value(""),     "HTTP service IP")
    ("SYS.HTTPPort",  value2(PortInt,     &m_HTTPPort)->default_value(0), "HTTP service Port")
    ("SYS.GeoipPath", value2(string,      &m_geoipPath)->default_value(""),     "Geoip path")
    ("SYS.SpecialIPPath", value2(string,      &m_specialIPPath)->default_value(""),     "Special IP path")
    ("SYS.ManamegentIP", value2(string,      &m_managementIP)->default_value(""),     "Manamegent IP")    
    ("MYSQL.Host", value2(string,      &m_host)->default_value(""),     "mysql host")
    ("MYSQL.User", value2(string,      &m_user)->default_value(""),     "mysql user")
    ("MYSQL.Password", value2(string,      &m_password)->default_value(""),     "mysql password")
    ("MYSQL.DB", value2(string,      &m_db)->default_value(""),     "mysql db")

    // Add options at here - end
    ;
}

void AccessServerConfig::measure(boost::shared_ptr<program_options::option_description> od)
{
    if (m_w1 < od->long_name().size())
        m_w1 = od->long_name().size();
        
    const ValueHelper* vh = dynamic_cast <const ValueHelper*>(od->semantic().get());
    
    if (m_w2 < vh->defaultValue().size())
        m_w2 = vh->defaultValue().size();
        
    if (m_w3 < vh->typeName().size())
        m_w3 = vh->typeName().size();
}

void AccessServerConfig::dumpConfig(const string& configCaption)
{
    if (m_out && *m_out)
    {
        *m_out << "# Configuration file for " << configCaption << "." << endl;
        *m_out << "# Alcatel-Lucent copyright. " << __DATE__ << endl;
        *m_out << "#" << endl;
        *m_out << "# use \"#\" to comment out a line." << endl;
        *m_out << "# Assign a value in following format." << endl;
        *m_out << "#     name = value" << endl;
        *m_out << "# If the parameter's name contains \".\", it could be organized by blocks." << endl;
        *m_out << "#     SYS.MemorySize = 1024" << endl;
        *m_out << "# 	 or" << endl;
        *m_out << "#     [SYS]" << endl;
        *m_out << "#     MemorySize = 1024" << endl;
        *m_out << "#     The scope of PositiveInt type is -1,0 and positive integer," << endl;
        *m_out << "#     -1 represents unlimited positive integer."<<endl;
        *m_out << "#     The scope of PositiveInt type is 1~65535."<<endl;
        *m_out << endl;
        *m_out << endl;
        for_each(m_desc.options().begin(), m_desc.options().end(), bind1st(mem_fun(&AccessServerConfig::measure), this));
        for_each(m_desc.options().begin(), m_desc.options().end(), bind1st(mem_fun(&AccessServerConfig::dumpParameter), this));
    }
}

void AccessServerConfig::dumpParameter(boost::shared_ptr<option_description> od)
{
    const ValueHelper* vh = dynamic_cast <const ValueHelper*>(od->semantic().get());
    *m_out << left << setw(m_w1) << od->long_name() << " = " << setw(m_w2 + 8) << vh->defaultValue()
           << setw(m_w3 + 6) << (string("# (") + vh->typeName() + ") ") << od->description() << endl;
}

void AccessServerConfig::checkMissingOption(boost::shared_ptr<option_description> od)
{
    if (m_vm[od->long_name()].empty())
        throw_exception(error(("Missing option: [" + od->long_name() + "].").c_str()));
}

void AccessServerConfig::dumpConfig(const string& configCaption, ostream& out)
{
    AccessServerConfig sc;
    sc.m_out = &out;
    sc.dumpConfig(configCaption);
}

void AccessServerConfig::reload(istream& in)
{
    if (in)
    {
    	boost::mutex::scoped_lock lk(m_mutexVM);
        m_vm = boost::program_options::variables_map();
        store(parse_config_file(in, m_desc), m_vm);
        notify(m_vm);
        for_each(m_desc.options().begin(), m_desc.options().end(), bind1st(mem_fun(&AccessServerConfig::checkMissingOption), this));
    }
    else
    {
        //throw IOException();
    }
}

}
}
}
}
