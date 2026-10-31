simple Balancer
{
    parameters:
        double timeBetweenCustomers @unit(s);
    gates:
        output out[];
}

simple Register
{
    parameters:
        double timePerItem @unit(s);
    gates:
        input in;
}

network Store
{
    types:
        channel Channel extends ned.DelayChannel { delay = 0ms; }
    submodules:
        register[4]: Register;
        balancer: Balancer;
    connections:
        balancer.out++ --> Channel --> register[0].in;
        balancer.out++ --> Channel --> register[1].in;
        balancer.out++ --> Channel --> register[2].in;
        balancer.out++ --> Channel --> register[3].in;
}