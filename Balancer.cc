#include <map>
#include <string.h>
#include <stdio.h>
#include <omnetpp.h>
#include "customer_m.h"
#include "queue_m.h"
#include <map>

using namespace omnetpp;


class Balancer : public cSimpleModule
{
private:
    simtime_t timeBetweenCustomers;
    int availableRegisters = 0;
    std::map<int, int> registerQueueSizes{{0,0}};
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
    if (std::string(msg->getName()) == "customer") {
        Customer* customer = check_and_cast<Customer*>(msg);

        int target = chooseTarget(customer);
        send(customer, "out", target);
        scheduleAt(simTime() + timeBetweenCustomers , createCustomer());
    } else if (std::string(msg->getName()) == "queue") {
        QueueMsg* queue = check_and_cast<QueueMsg*>(msg);

        registerQueueSizes[queue->getArrivalGate()->getIndex()] = queue->getTotalItems();
        delete queue;
    }
}

int Balancer::chooseTarget(Customer* customer)
{
    int target = 0;
    int lowest = registerQueueSizes[target];
    for (int i = 1; i < registerQueueSizes.size(); i++) {
        if (registerQueueSizes[i] < lowest) { target = i; }
    }
    // TODO: this is just random for now
    return target;
}

void Balancer::finish()
{
    //Nothing
}
