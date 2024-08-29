# Unique ID Generator

A unique ID generator server using CORBA.

* IDs are long integers (limited to 10,000,000,000)
* IDs can be returned to the server and, may therefore, be served to (another) client (`putback` method) - raises `InvalidID` exception if ID hasn't been previously served
* `reset` -- starts over (past IDs served may be served again) -- default password="uid" (returns true, if reset was successful)
* `getustr` -- generates a 12 charcter unique string (strings cannot be returned to the server)

Server uses CORBA's **naming service** that must be running.

## Interface of the Distributed Object

```idl
module UIDGen {

    typedef long ID_t;

    exception InvalidID {};

    interface UniqueIDGen {
        ID_t getuid();
        void putback(in ID_t id) raises (InvalidID);
        bool reset(in string passwd);
        string getustr();
    };

};
```

## Library Dependencies

* [libfmt](https://github.com/fmtlib/fmt)
* [spdlog](https://github.com/gabime/spdlog)
* [ACE/TAO CORBA](https://github.com/DOCGroup/ACE_TAO)
* [Common Object Library - colibry](https://github.com/laplima/colibry)
