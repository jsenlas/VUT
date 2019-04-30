Feature: Product management 

    Background:
        Given Admin is logged in

    Scenario: Product page
        Given admin is at Dashboard webpage
        When admin chooses category Products from Catalog
        Then Product page is shown

    Scenario: Add new product
        Given admin is at products webpage
        When admin clicks on the + button
        Then form for creating new product is shown

    Scenario: Fill in required data
        Given Add Product page is loaded
        When admin fills in the form
        And admin presses save button
        Then new product is in product list
    
    Scenario: Select and copy product
        Given administrator is at products webpage
        When admin selects one of the products
        And admin presses copy button
        Then the same product shows in the list

    Scenario: Delete
        Given a product is selected
        When admin presses delete button
        Then the product is not in the list
    
    Scenario: Edit
        Given admin is at products webpage
        When admin presses edit button at one of the products
        Then Edit Product page is loaded

    Scenario: Edit product, Save
        Given admin is at Edit product page
        When admin finishes all changes 
        And admin presses save button
        Then product list is shown
        
    Scenario: Filter
        Given admin is at products webpage
        When admin fills in filter details
        And presses the Filter button
        Then relevant products are shown in the list

    Scenario Outline: Sort
        Given admin is at products webpage
        When admin presses "<table_cell>"
        Then the list is sorted according to the category
    Examples:
        | table_cell    |
        | Product Name  |
        | Model         |
        | Price         |
        | Quantity      |
        | Status        |
        