# SiREN v2.0
## _The Similarity Retrieval Engine v2.0_

SIREN is a *middleware* software for Relational Database Management Systems proposed by [Barioni et al. (2006)][sirenpaper]. SIREN's core ideas are *(i)* the seamless extension of the SQL language towards similarity searching, and *(ii)* the definition of a proper extension for *selection* and *join* operators. Such concepts were further extended by [theoretical][sirenopt] and [experimental][medfmisir] studies. 

The first SIREN implementation provided an extension for DDL commands involving comparisons by similarity and an interpreter for `SELECT` commands. The *selection* predicates involving similarity were solved by the interpreter and translated into `IN`-list conditions. The implementation in this repository:

1. Modifies the [SIREN grammar][newgrammar];
2. Adds support for sub-selects (See [query examples][examples]);
3. Adds support for [diversified similarity searching][divsearch] predicates;
4. Provides a client-server implementation for running SIREN over underlying DBMSs' [MySQL][mysql] and [SQLite][sqlite]. 

## Features

- Client-server implementation (through QTCP and QSQL)
- Querying support for MySQL and SQLite
- Support for multidimensional feature vectors
- Support for multiple distance functions
- Support for metric indexes
- Support for similarity searching - *k*-Nearest Neighbors queries
- Support for similarity searching - Range queries
- Support for diversified similarity searching - BRID queries
- Support for diversified similarity searching - BRIDGE queries
- ✨Combine order and identity predicates with distance-based conditions!✨


> SIREN assumes similarity predicates have priority for execution (written order). Therefore, mind issuing non-commutative conditions carefully (*e.g.*, kNN) or use nested subselects to fix the execution order for inner->outer conditions.

## Background technologies and 3rd-party libraries

Siren relies on a series of background technologies and 3rd-party libraries to work properly, namely:

1. Background technologies

- [Qt SDK][qt] - Qt Cross-Platform and SDK C++ development environment.
- [Agathon library][agathon] - Self-contained library with tokenizer and interpreter for similarity searching predicates.

2. 3rd-party libraries

- [Hermes][hermes] - Our own C++ library of distance functions (and utils).
- [Arboretum (forked)][arbforked] - A forked version of the Arboretum library for metric indexing that supports BRIDGE (master [here][arboretum])


## Installation

1. Install the background technologies and download the source code of 3rd-party libraries.
Then, adjust the include paths of `siren.pro.example` file to point to the installation folders of the 3rd-party libraries and rename it to `siren.pro`.
After that, you can use `build.sh` to compile the server version.

```sh
cd siren
./build.sh && make clean
```

2. Run the compiled binary by providing the server IP, port, and the DBMS type and user (*e.g.*, localhost server running on port 3434 for MySQL with user 'user').
```sh
cd build-siren #your compiled binary folder location
./Siren-Server -u user -p 3434 -h localhost -d MySQL  #requires MySQL to be installed
#./Siren-Server -u user -p 3434 -h localhost          #requires SQLite to be installed
```

> SIREN over SQLite does not offer support diversified similarity searching because SQLite interpreter does not handle double IN-list conditions.

3. Run the [Siren DDL script][sirenddl] by issuing the DDL commands through a client (*e.g.*, telnet) for the server.
> You can use a simple telnet connection (i.e., telnet <siren-server-ip> <siren-server-port>) to submit the DDL script that contains standard SQL commands for the creation of SIREN internal tables.

## Directory structure

The directory tree is structured as follows.

| Dir | Description |
| ------ | ------ |
| root  | Main functionalities and util. |
| example | Include examples and cases of use for SIREN. |
| 3rd-party  | Includes the entry points for 3rd-party libraries. |
| DDL | Includes the DDL script for SIREN instantiation.  |


## Development

Future directions for SIREN include adding support to [Apache Impala][impala] and the extension of its query language interpreter to [Apache Thrift][thrift].


## Notes

- SIREN is NOT commercial software. **It is built for education and demonstration purposes ONLY!**
- If you find any bug, feel free to open an issue. 
- *The code is provided 'as is' and without any type of warrant or whatsoever (including bug fixing) - See License*.


[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)

   [sirenpaper]: <https://dl.acm.org/doi/abs/10.5555/1182635.1164232>
   [sirenopt]: <https://hal.archives-ouvertes.fr/hal-00687320/file/JIDM_2011.pdf>
   [medfmisir]: <https://link.springer.com/chapter/10.1007/978-3-642-23208-4_2>
   [mysql]: <https://www.mysql.com/>
   [sqlite]: <https://www.sqlite.org/index.html>
   [newgrammar]: <https://www.sqlite.org/index.html>
   [divsearch]: <https://www.sqlite.org/index.html>
   [examples]: <https://www.sqlite.org/index.html>
   [qt]: <https://www.qt.io/download>
   [arboretum]: <https://bitbucket.org/gbdi/arboretum/src/master/>
   [hermes]: <https://github.com/marcosivni/hermes>
   [arbforked]: <https://github.com/marcosivni/arboretum>
   [agathon]: <www>
   [sirenddl]: <www>
   [impala]: <https://impala.apache.org/>
   [thrift]: <https://thrift.apache.org/>
 
