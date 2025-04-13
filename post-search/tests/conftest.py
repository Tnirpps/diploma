import pathlib
import sys

import post_search_pb2_grpc as post_search_services
import pytest


pytest_plugins = [
    'pytest_userver.plugins.core',
    'pytest_userver.plugins.grpc',
]
USERVER_CONFIG_HOOKS = []


@pytest.fixture
def grpc_service(grpc_channel, service_client):
    return post_search_services.ProtoConverterStub(grpc_channel)


def pytest_configure(config):
    sys.path.append(str(pathlib.Path(__file__).parent.parent / 'proto/'))
