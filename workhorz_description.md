# WorkHorz-Server

WorkHorz is a web-application-engine in modern C++ with an integrated bare-essentials webserver, a LUA scripting 
language plugin ability fully integrated (more later). Works great for big enterprise-sized web-applications (like 
Saas) that need top performance, high speed, high security, easy scalability and easy integration into existing OS 
and libraries offering full control over their web-app. But is also adaptable to small tools for Devops and small 
to mid-sized web-apps. Its modular architecture allows you to tailor it to your needs and exclude what you don't 
need. The principle is: _Only pay for what you use!_ Also, it allows the users to hot-swap between version (on a 
component level or whole app) to test out things or make the deployment faster.


The Open-Source version is of course free of charge. Business users should consider the commercial license which 
frees them to share the source code.

## What makes WorkHorz great and unique?
The primary goal is to make the development of web-applications easier and faster. The key features that make it 
great are the following points that you won't find in other web-application-engines or frameworks in this combination:

* **Scalability**: The modular architecture allows you to scale the application to your needs. You can exclude what 
  you don't need and add what you need. This makes the application faster and generally more secure. Also, you can 
  easily
configure a minion-master node setup for load balancing that can be placed anywhere in the world.
* **Performance**: As it's written in modern C++ and uses the best libraries available, it was designed to deliver 
  top performance that is needed for anything between an embedded app to a full enterprise-sized web application. 
  Architecture and code design have been optimized for speed and performance. And besides C++ being the fastest 
  language currently also has the principle that you only pay for what you use - which leaves you in full control of 
  your performance. The goal is to serve a Million requests per second on a single server (yes that's _1'000'000 
  Req/Sec_).
* **Security**: The engine is designed with security in mind in all its components and nodes. Reliable security 
  features are made available to you to use to cover all levels of security from lose to paranoid.
* **Ease of use**: The engine is designed to be easy to use. If you choose the basic setup, you can write your web 
  application with just a few lines of code. And if you're not proficient in C++ or have no time to go deep into it, 
  you can use the LUA instead to write your webapp. The LUA scripting language is easy to learn and understand and 
  has lots of useful libraries. Another intended approach is to split up your teams into the C++ foundation team and 
  the LUA business logic team. The LUA scripting language plugin allows you to write scripts in a simple and 
  easy-to-understand language.
* **Integration**: Integrations into external webservices are crucial nowadays, and you may find yourself in need of 
  a number of them to add to your webservice. This should not be hard to do and indeed with WorkHorz it's built-in 
  and easy to do and since it comes with a lot of integrations ready to use for you, it may save you weeks of work 
  and testing from the start.
* **Monitoring**: Making monitoring easy and painless was a big goal. The integrated monitoring tools allow you to 
  monitor the basic core functions of your webapp and you can easily add more to it. It relies on modern industry 
  standard monitoring tools like Prometheus and Grafana and therefore fits seamlessly into your existing monitoring 
  infrastructure.
* **Deployment**: Deployment shouldn't be hard to do. With the integrated Docker/Kubernetes support, you can deploy 
  your webapp in minutes. The integrated tools allow you to easily deploy your webapp to your servers and keep it 
  up-to-date. And this way, it's also easy to integrate into your CD/CI pipeline. 

##  Architecture
... 

## Example Use-Cases
...

## Features (roughly)
These are the features and nodes that are planned for the first phase. Depending on the input and experience with it, 
these may change over time. Docker and/or Kubernetes containers will make the deployment in enterprise servers 
easier.

- [x] Nodes and Components
    - [ ] Scalable by design (on components level), and easier to keep it scalable
    - [ ] Template Cacher
    - [ ] Webserver (HTTP/HTTPS)
        - [ ] Websockets-Server
        - [ ] Push-Server
    - [ ] LUA Plugin Mgmt. (later more interpreted languages like https://github.com/cesanta/mjs)
        - [ ] C# integration
    - [ ] Security Mgmt. + securing scripts
    - [ ] MVC (Server) & MCCV (Server + Client) Paradigm
    - [ ] (WASI integration (https://wasi.dev/))
    - [ ] Load Balancing Mgmt.
    - [ ] OS & Utility Module
    - [ ] Session Mgmt.
    - [ ] Application API (for the customer WebApp)
    - [ ] Logging (to text files and InfluxDB)
    - [ ] REST-API construction and scaffolding
    - [ ] (GraphQL queries)
    - [ ] RDBMS ORM access to the most used database engines (https://github.com/SOCI/soci)
        - [ ] Active Records Pattern integrated for ease of use
    - [ ] Compression outside of HTTP (https://github.com/rikyoz/bit7z)
    - [ ] Integration: Easy to add new API integrations to external web services
    - [ ] Apache and nginx compatible configuration of the integrated webserver (highest performance)
        - [ ] Webapps can also run under those with FastCGI if desired (httpd and TLS termination as proxy service)
    - [ ] Use of external caching like with Redis, memcached or others
    - [ ] Licensing server and management
    - [ ] Marketplace for 3rd party components
    - [ ] Easy translation of webpages and content
    - [ ] (Chatbots integration (like http://reo7sp.github.io/tgbot-cpp))
    - [ ] JWT Json Web Tokens
    - [ ] Login/Registration generation and log aggregation
        - [ ] Measurements for server monitoring  for tools like Grafana, etc.
    - [ ] Auth/Permission generation
    - [ ] L8n and I18n based on ICU
    - [ ] SRL for backend and frontend https://simple-regex.com/
    - [ ] Fast Routing for thousands of pages/paths (searching is fast too)
    - [ ] ToolCase (mostly external units)
        - [ ] Scaffolding
        - [ ] WebApp Config Mgmt.
        - [ ] Build & Deployment Mgmt.
        - [ ] Unit-Testing
- [x] Administration and Monitoring
    - [ ] Web-GUI administration and configuration (remote access)
    - [ ] Integration into monitoring solutions like Prometheus/Grafana, Zabbix and the likes
    - [ ] Visual plugin management
    - [ ] Visual permission management
    - [ ] Browse log files easier with smart filtering to find what you need faster

## Technical Requirements
I want to use pure C++23/26 wherever possible and sound. I do this intending to make the development and maintenance 
easier and the resulting binaries faster. But it is also equally sensible to use good and well-tested libraries that 
offer features which would otherwise be difficult or too lengthy to write myself. Out of all this, I have the 
following list of required technical tools/libs:

* **C++23**: GCC >= 14.x or CLANG >= 19.x, MSVC >= 2022
* Intended OS platforms: Linux, OpenBSD, Mac OS X - (development as of now mainly on Linux)
* Boost >= 1.85 (to be tested once officially released)
    * perhaps LockFree?
    * ??
* ORM for DB access (SQLite for the moment, more coming later)
* Catch2 >= 3.6 (testing framework BDD, header-only https://github.com/catchorg/Catch2)
* Quill >= 5.1 (logging lib https://github.com/odygrd/quill)
* LUA >= 5.4.7 (cpp wrapper lib Sol2 https://github.com/ThePhD/sol2)
* OpenSSL >= 3.3
* dOxygen >= 1.11
* fmt >= 11.x (fast string formatting lib, https://github.com/fmtlib/fmt/)
* CMake >= 3.28
* ninja >= 1.11.x (build tool, https://ninja-build.org/)
* nghttp2 >= 1.62.x (HTTP/2 lib, https://nghttp2.org/ with https://github.com/nghttp2/nghttp2)
* libevent >= 2.1 (event loop lib, https://libevent.org/, for now)
* io_uring >= 0.9 (for Linux event handling, https://kernel.dk/io_uring.pdf)
* Monitoring integration (like https://prometheus.io/ with https://github.com/jupp0r/prometheus-cpp)
* Testing generation https://github.com/emil-e/rapidcheck
* Docker/Kubernetes integration (https://www.docker.io/)
* ZeroMQ >= 4.3 (messaging lib, https://zeromq.org/)
* Pulsar (for messaging and qeueing https://pulsar.apache.org/)
* TeamCity (CI/CD tool, https://www.jetbrains.com/teamcity/)
* Coverity Scan (maybe, Code Analysis, https://scan.coverity.com/)
* CPPCheck (Code Analysis, http://cppcheck.sourceforge.net/)
* Aqua Testing (Testing Automation https://www.jetbrains.com/aqua/)
* Quodana (Static Code Analysis https://www.jetbrains.com/qodana/)
* Mockoon (API Mocking Tool https://mockoon.com/)
* ...

## Help Needed
I will greatly welcome help at any stage of the process. If you have any expertise that would help make progress in 
a module, please do contact me.
* Building a simple blog website served by WorkHorz
* Template Parser and Cacher
* Request handling concurrently (maybe with TaskFlow)

## Trials
* Kanbanery Integration in GitHub to plan the tasks at hand better.
* ...

## Integration & Development
I'm trying out Travis-CI and Coverity Scan.
* Travis-CI status: ![Build Status](https://travis-ci.org/RedSilkSoftware/workhorz-server.svg?branch=master)
* Coverity Scan (Code Analysis) status: ...

### Development Process
Vagrant could be used as a development and/or platform-testing environment. Needs more research.
* [Vagrant](http://docs.vagrantup.com/)

### Deployment
Docker/Kubernetes (or Lmctfy) will be a great way to provision WorkHorz quickly to any server. It is an ultimate 
goal of deliverables. Needs more research.
* [Docker](https://www.docker.io/)
* [Let Me Contain That For You](https://github.com/google/lmctfy)
