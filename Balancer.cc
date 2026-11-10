#include <string.h>
#include <stdio.h>
#include <omnetpp.h>
#include "customer_m.h"

using namespace omnetpp;


class Balancer : public cSimpleModule
{
private:
    simtime_t timeBetweenCustomers;
    int availableRegisters = 0;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual int chooseTarget(Customer* customer);
    virtual Customer* createCustomer();
    virtual void finish();
};

Define_Module(Balancer);

Customer* Balancer::createCustomer()
{
    // Randomize here, cause i couldnt figure out to do it from the .msg or .ini
    auto* customer = new Customer("customer");
    customer->setRemainingItems(std::max((int)truncnormal(13,4), 1));
    customer->setTimePerItem(truncnormal(1.5, 0.3));
    return customer;
}

void Balancer::initialize()
{
    timeBetweenCustomers = par("timeBetweenCustomers");
    availableRegisters = gateSize("out");

    scheduleAt(0.0, createCustomer());
}

void Balancer::handleMessage(cMessage* msg)
{
    Customer* customer = check_and_cast<Customer*>(msg);

    int target = chooseTarget(customer);
    send(customer, "out", target);
    scheduleAt(simTime() + exponential(timeBetweenCustomers) , createCustomer());
}

int Balancer::chooseTarget(Customer* customer)
{
    // TODO: this is just random for now
    return intuniform(0,availableRegisters-1);
}

void Balancer::finish()
{
    //Nothing
}
