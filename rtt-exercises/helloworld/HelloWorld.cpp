
/**
 * @file HelloWorld.cpp
 * This file demonstrate each Orocos primitive with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/TaskContext.hpp>
#include <ocl/TaskBrowser.hpp>
#include <rtt/Logger.hpp>
#include <rtt/Property.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/Method.hpp>
#include <rtt/Port.hpp>
#include <rtt/Activity.hpp>

#include <ocl/OCL.hpp>

using namespace std;
using namespace RTT;
using namespace Orocos;

namespace Example
{

    /**
     * Every component inherits from the 'TaskContext' class.  This base
     * class allow a user to add a primitive to the interface and contain
     * an ExecutionEngine which executes application code.
     */
    class HelloWorld
        : public TaskContext
    {
    protected:
        /**
         * @name Name-Value parameters
         * @{
         */
        /**
         * Properties take a name, a value and a description
         * and are suitable for XML.
         */
        Property<std::string> property;
        /**
         * Attributes are aliased to class variables.
         */
        std::string attribute;
        /**
         * Constants are aliased, but can only be changed
         * from the component itself.
         */
        std::string constant;
        /** @} */

        /**
         * @name Input-Output ports
         * @{
         */
        /**
         * We publish our data through this OutputPort
         *
         */
        OutputPort<std::string> outport;
        /**
         * This InputPort buffers incoming data.
         */
        InputPort<std::string> inport;
        /** @} */

        /**
         * An operation we want to add to our interface.
         */
        std::string mymethod() {
            return "Hello World";
        }

        /**
         * This one is executed in our own thread.
         */
        bool sayWorld( std::string word) {
            cout <<"Saying Hello '"<<word<<"' in own thread." <<endl;
            if (word == "World")
                return true;
            return false;
        }

        void updateHook() {
            //cout << "."<<endl;
        }
    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        HelloWorld(std::string name)
            : TaskContext(name),
              // Name, description, value
              property("the_property", "the_property Description", "Hello World"),
              attribute("Hello World"),
              constant("Hello World"),
              // Name, initial value
              outport("outport",true),
              // Name, policy
              inport("inport",ConnPolicy::buffer(13,ConnPolicy::LOCK_FREE,true) )
        {
            // New activity with period 0.01s and priority 0.
            this->setActivity( new Activity(0, 0.01) );

            // Set log level more verbose than default,
            // such that we can see output :
            if ( log().getLogLevel() < Logger::Info ) {
                log().setLogLevel( Logger::Info );
                log(Info) << "HelloWorld manually raises LogLevel to 'Info' (5). See also file 'orocos.log'."<<endlog();
            }

            // Check if all initialisation was ok:
            assert( property.ready() );

            // Now add it to the interface:
            this->addProperty(&property);
            this->addAlias("the_attribute", attribute);
            this->addConstAlias("the_constant", constant);

            this->ports()->addPort(&outport);
            this->ports()->addPort(&inport);

            this->addOperation( "the_method", &HelloWorld::mymethod, this, ClientThread ).doc("'the_method' Description");

            this->addOperation( "the_command", &HelloWorld::sayWorld, this, OwnThread).doc("'the_command' Description").arg("the_arg", "Use 'World' as argument to make the command succeed.");

            log(Info) << "**** Starting the 'Hello' component ****" <<endlog();
            // Start the component's activity:
            this->start();
        }
    };
}


// This define allows to compile the hello world component as a library
// liborocos-helloworld.so or as a program (helloworld). Your component
// should only be compiled as a library.
#ifndef EXAMPLE_COMPONENT_ONLY

int ORO_main(int argc, char** argv)
{
    Logger::In in("main()");

    // Set log level more verbose than default,
    // such that we can see output :
    if ( log().getLogLevel() < Logger::Info ) {
        log().setLogLevel( Logger::Info );
        log(Info) << argv[0] << " manually raises LogLevel to 'Info' (5). See also file 'orocos.log'."<<endlog();
    }

    log(Info) << "**** Creating the 'Hello' component ****" <<endlog();
    // Create the task:
    Example::HelloWorld hello("Hello");

    log(Info) << "**** Using the 'Hello' component    ****" <<endlog();

    // Do some 'client' calls :
    log(Info) << "**** Reading a Property:            ****" <<endlog();
    Property<std::string> p = hello.properties()->getProperty<std::string>("the_property");
    assert( p.ready() );
    log(Info) << "     "<<p.getName() << " = " << p.value() <<endlog();

    log(Info) << "**** Sending a Method:             ****" <<endlog();
    Method<bool(std::string)> c = hello.getOperation<bool(std::string)>("the_command");
    assert( c.ready() );
    log(Info) << "     Sending Method : " << c.send("World")<<endlog();

    log(Info) << "**** Calling a Method:              ****" <<endlog();
    Method<std::string(void)> m = hello.getOperation<std::string(void)>("the_method");
    assert( m.ready() );
    log(Info) << "     Calling Method : " << m() << endlog();

    log(Info) << "**** Starting the TaskBrowser       ****" <<endlog();
    // Switch to user-interactive mode.
    OCL::TaskBrowser browser( &hello );

    // Accept user commands from console.
    browser.loop();

    return 0;
}

#else

#include "ocl/ComponentLoader.hpp"
ORO_CREATE_COMPONENT( Example::HelloWorld )

#endif
