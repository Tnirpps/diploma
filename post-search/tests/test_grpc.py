# test_post_search.py

import post_search_pb2 as protos
import json
import pytest

# Basic search result test
async def test_convert_search_results(grpc_service):
    request = protos.InputMessage()

    # Set up search result
    result = request.result
    result.total_items = 10

    item = result.items.add()
    item.id = "item1"
    item.title = "Test Product"
    item.description = "This is a test product description"
    item.rating = 4.5

    # Call the service
    response = await grpc_service.ConvertToJson(request)

    # Validate response
    assert response.success is True
    assert not response.error

    json_data = json.loads(response.json_data)
    assert "search_results" in json_data
    assert json_data["search_results"]["total_items"] == 10
    assert len(json_data["search_results"]["items"]) == 1
    assert json_data["search_results"]["items"][0]["id"] == "item1"
    assert json_data["search_results"]["items"][0]["rating"]["value"] == 4.5
    assert json_data["search_results"]["items"][0]["rating"]["stars"] == "★★★★★"

# Category test
async def test_convert_category(grpc_service):
    request = protos.InputMessage()

    # Set up category
    category = request.category
    category.id = "electronics"
    category.name = "Electronics"
    category.path = "/goods/electronics"

    # Add parent category
    parent = category.parent
    parent.id = "goods"
    parent.name = "Goods"

    # Call service
    response = await grpc_service.ConvertToJson(request)

    # Validate response
    assert response.success is True

    json_data = json.loads(response.json_data)
    assert "category" in json_data
    assert json_data["category"]["id"] == "electronics"
    assert json_data["category"]["name"] == "Electronics"
    assert json_data["category"]["path"] == "/goods/electronics"
    assert json_data["category"]["parent"]["id"] == "goods"
    assert json_data["category"]["parent"]["name"] == "Goods"

# Price test including discount calculation
async def test_convert_prices(grpc_service):
    request = protos.InputMessage()

    # Regular price
    price1 = request.price.add()
    price1.amount = 100.0
    price1.currency = "USD"

    # Discounted price
    price2 = request.price.add()
    price2.amount = 200.0
    price2.currency = "EUR"
    price2.discount = 0.25  # 25% discount

    # Call service
    response = await grpc_service.ConvertToJson(request)

    # Validate response
    assert response.success is True

    json_data = json.loads(response.json_data)
    assert "prices" in json_data
    assert len(json_data["prices"]) == 2

    # Regular price validation
    assert json_data["prices"][0]["amount"] == 100.0
    assert json_data["prices"][0]["currency"] == "USD"
    assert "discount_percent" not in json_data["prices"][0]

    # Discounted price validation
    assert json_data["prices"][1]["amount"] == 200.0
    assert json_data["prices"][1]["currency"] == "EUR"
    assert json_data["prices"][1]["discount_percent"] == 25.0
    assert json_data["prices"][1]["discounted_amount"] == 150.0  # 200 * (1-0.25)

# Favorite categories with preference levels
async def test_convert_favorite_categories(grpc_service):
    request = protos.InputMessage()

    # High preference category
    fav1 = request.favourite_category.add()
    fav1.id = "electronics"
    fav1.name = "Electronics"
    fav1.preference_score = 0.9

    # Medium preference category
    fav2 = request.favourite_category.add()
    fav2.id = "books"
    fav2.name = "Books"
    fav2.preference_score = 0.7

    # Low preference category
    fav3 = request.favourite_category.add()
    fav3.id = "garden"
    fav3.name = "Garden"
    fav3.preference_score = 0.3

    # Call service
    response = await grpc_service.ConvertToJson(request)

    # Validate response
    assert response.success is True

    json_data = json.loads(response.json_data)
    assert "favourite_categories" in json_data
    assert len(json_data["favourite_categories"]) == 3

    # Check preference levels transformation
    assert json_data["favourite_categories"][0]["id"] == "electronics"
    assert json_data["favourite_categories"][0]["preference_level"] == "high"

    assert json_data["favourite_categories"][1]["id"] == "books"
    assert json_data["favourite_categories"][1]["preference_level"] == "medium"

    assert json_data["favourite_categories"][2]["id"] == "garden"
    assert json_data["favourite_categories"][2]["preference_level"] == "low"

# User data with privacy protection
async def test_convert_user_data(grpc_service):
    request = protos.InputMessage()

    # Set up user
    user = request.user
    user.id = "user123"
    user.email = "test.user@example.com"
    user.preferences.SetInParent()  # Mark that preferences exist

    # Call service
    response = await grpc_service.ConvertToJson(request)

    # Validate response
    assert response.success is True

    json_data = json.loads(response.json_data)
    assert "user" in json_data
    assert json_data["user"]["id"] == "user123"
    assert json_data["user"]["email"] == "te***@example.com"  # Email should be masked
    assert json_data["user"]["has_personalization"] is True

# Comprehensive test with all fields
async def test_convert_comprehensive(grpc_service):
    request = protos.InputMessage()

    # Add search results
    result = request.result
    result.total_items = 25

    # Add items to search results
    item1 = result.items.add()
    item1.id = "item1"
    item1.title = "Smartphone XYZ"
    item1.description = "Latest smartphone model with great features"
    item1.rating = 4.5

    item2 = result.items.add()
    item2.id = "item2"
    item2.title = "Laptop ABC"
    item2.description = "Powerful laptop for work and gaming"
    item2.rating = 3.5

    # Add category
    category = request.category
    category.id = "electronics"
    category.name = "Electronics"
    category.path = "/goods/electronics"

    # Add price
    price = request.price.add()
    price.amount = 1299.99
    price.currency = "USD"
    price.discount = 0.15  # 15% discount

    # Add favorite category
    fav = request.favourite_category.add()
    fav.id = "electronics"
    fav.name = "Electronics"
    fav.preference_score = 0.85

    # Add user
    user = request.user
    user.id = "user456"
    user.email = "john.doe@example.com"
    user.preferences.SetInParent()

    # Call service
    response = await grpc_service.ConvertToJson(request)

    # Validate response structure
    assert response.success is True
    assert not response.error

    json_data = json.loads(response.json_data)

    # Check that all sections are present
    assert "search_results" in json_data
    assert "category" in json_data
    assert "prices" in json_data
    assert "favourite_categories" in json_data
    assert "user" in json_data

    # Validate a few key fields
    assert json_data["search_results"]["total_items"] == 25
    assert len(json_data["search_results"]["items"]) == 2
    assert json_data["prices"][0]["discounted_amount"] == pytest.approx(1104.99)
    assert json_data["favourite_categories"][0]["preference_level"] == "high"
    assert json_data["user"]["email"] == "jo***@example.com"

# Error handling test
async def test_error_handling(grpc_service, monkeypatch):
    request = protos.InputMessage()

    # Force an exception during processing
    def mock_process(*args, **kwargs):
        raise RuntimeError("Test exception")

    # Apply the mock (actual method name will depend on your implementation)
    monkeypatch.setattr(
        "post_search.Renderer.ConvertToJson",
        mock_process
    )

    # The test should either:
    # 1. Get a gRPC error (INTERNAL) or
    # 2. Get a response with success=False

    try:
        response = await grpc_service.ConvertToJson(request)
        # If we get here, we should have a failure response
        assert response.success is False
        assert "Failed to convert" in response.error
    except Exception as e:
        # If we get an exception, it should be a gRPC error
        assert "INTERNAL" in str(e)
