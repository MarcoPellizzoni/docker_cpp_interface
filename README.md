# DOCKER API

This API aims to enable the use of docker (and with some future minor adjustment Podman) from a C++ Code.

The goal is to interact with containers the same way you would do from a terminal. So that the commands, and the way they are constructed, are the same as the ones you normally write on the terminal.

##### Note:
This API was particulary usefull to me working on a project where I needed containers to be managed at runtime, but I didn't want the complexity nor the dependency from a third party orchestrator.
