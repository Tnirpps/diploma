import pathlib
import sys 

import handlers.hello_pb2_grpc as hello_services 
import pytest


pytest_plugins = [
    'pytest_userver.plugins.core',
    'pytest_userver.plugins.grpc', 
]
USERVER_CONFIG_HOOKS = [
    # 'prepare_service_config_grpc', 
]




@pytest.fixture
def grpc_service(grpc_channel, service_client):
    return hello_services.HelloServiceStub(grpc_channel)


def pytest_configure(config):
    sys.path.append(str(pathlib.Path(__file__).parent.parent / 'proto/handlers/'))



