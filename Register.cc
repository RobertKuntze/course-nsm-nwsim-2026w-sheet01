#include <string.h>
#include <stdio.h>
#include <omnetpp.h>
#include "customer_m.h"
#include <queue>

using namespace omnetpp;


class Register : public cSimpleModule
{
private:
    simtime_t timePerItem;
    bool useRegisterTimePerItem;
    std::queue<Customer*> queue;
    cMessage* working = new cMessage("working"); // used to abstract when the register is in use
    cOutVector customerWaitTimeVector; // records time spent in queue (does not include waiting for items to be processed)
    cOutVector queueSizeVector; // records the queue size whenever a customer joins/leaves the queue
    simtime_t serviceTime = 0.0;
    cOutVector serviceTimeVector;
    cOutVector idleTimeVector;
protected:
virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual simtime_t calculateProcessTime(Customer* customer);
    virtual void finish();
};

Define_Module(Register);

void Register::initialize()
{
    timePerItem = par("timePerItem");
    useRegisterTimePerItem = true;
    WATCH(queue);
    
    customerWaitTimeVector.setName("customerWait");
    queueSizeVector.setName("queueSizeVector");
    serviceTimeVector.setName("serviceTimeVector");
    idleTimeVector.setName("idleTimeVector");
}

simtime_t Register::calculateProcessTime (Customer* customer)
{
    if (!useRegisterTimePerItem) {
        timePerItem = customer->getTimePerItem();
    }
    simtime_t totalTime = customer->getRemainingItems() * timePerItem;
    serviceTime += totalTime;
    serviceTimeVector.record(serviceTime);
    idleTimeVector.record(simTime() - serviceTime);

    return totalTime;
}

void Register::handleMessage(cMessage* msg)
{
    if (msg == working) {
        Customer* front = queue.front();
        
        delete front;
        queue.pop();
        
        if (!queue.empty()) {
            front = queue.front();
            simtime_t wait = simTime() - front->getWaitStart();
            customerWaitTimeVector.record(wait.dbl());
            scheduleAt(simTime() + calculateProcessTime(front), working);
            // bubble("Finished, starting again");
        }
        return;
    }
    // record the queue size, before the customer is added - represents
    queueSizeVector.record(queue.size());

    Customer* customer = check_and_cast<Customer*>(msg);
    queue.push(customer);
    // bubble("Got a new Customer");
    customer->setWaitStart(simTime());


    if (!working->isScheduled()) {
        // self message to handle customer
        scheduleAt(simTime() + calculateProcessTime(customer), working);
        // bubble("Starting after break");
    }
}

void Register::finish()
{
    cancelAndDelete(working);
    working = nullptr;

    queueSizeVector.record(queue.size());

    while (!queue.empty()) { delete queue.front(); queue.pop(); }
}