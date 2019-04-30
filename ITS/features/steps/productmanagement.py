from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
from selenium.common.exceptions import NoSuchElementException, TimeoutException
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

from behave import *
import time

# Feature: Product management
@given(u'Admin is logged in')
def step_impl(context):
    context.browser.get("http://mys01.fit.vutbr.cz:8059/admin/")
    # Log in the admin
    context.browser.find_element_by_id("input-username").send_keys("admin")
    context.browser.find_element_by_id("input-password").send_keys("admin")
    context.browser.find_element_by_xpath("//button[@type='submit']").click()

    time.sleep(5)
    # print(context.browser.current_url)
    context.my_token = context.browser.current_url.split("&")[1].split('=')[1]
    # print(context.my_token)
    assert (context.browser.title == "Dashboard")


# Scenario: Product page
@given(u'admin is at Dashboard webpage')
def step_impl(context):
    context.browser.get("http://mys01.fit.vutbr.cz:8059/admin/index.php?route=common/dashboard&token=" + context.my_token)
    time.sleep(15)
    print(context.browser.title)


@when(u'admin chooses category Products from Catalog')
def step_impl(context):
    element_to_hover_over = context.browser.find_element_by_link_text("Catalog")
    hover = webdriver.ActionChains(context.browser).move_to_element(element_to_hover_over)
    hover.perform()
    context.browser.find_element_by_xpath("//a[contains(text(),'Products')]").click()


@then(u'Product page is shown')
def step_impl(context):
    assert (context.browser.title == u"Products")


    # Scenario: Add new product
@given(u'admin is at products webpage')
def step_impl(context):
    context.browser.get("http://mys01.fit.vutbr.cz:8059/admin/index.php?route=catalog/product&token=" + context.my_token)
    assert context.browser.title == "Products"


@when(u'admin clicks on the + button')
def step_impl(context):
    context.browser.find_element_by_css_selector(".fa-plus").click()


@then(u'form for creating new product is shown')
def step_impl(context):
    t = context.browser.find_element_by_xpath("//h3").text
    assert t == "Add Product"


# Scenario: Fill in required data
@given(u'Add Product page is loaded')
def step_impl(context):
    context.browser.get("http://mys01.fit.vutbr.cz:8059/admin/index.php?route=catalog/product/add&token=" + context.my_token)


@when(u'admin fills in the form')
def step_impl(context):
    context.browser.find_element_by_xpath("//a[contains(text(),'General')]").click()
    context.browser.find_element_by_id("input-name1").send_keys("Super product")
    context.browser.find_element_by_id("input-meta-title1").send_keys("supertag")
    context.browser.find_element_by_xpath("//a[contains(text(),'Data')]").click()
    context.browser.find_element_by_id("input-model").send_keys("supermodel")
    context.browser.find_element_by_xpath("//a[contains(text(),'Links')]").click()
    context.browser.find_element_by_id("input-manufacturer").send_keys("supermen")
    context.browser.find_element_by_xpath("//button[@type='submit']").click()


@when(u'admin presses save button')
def step_impl(context):
    context.browser.find_element_by_id("form-product").submit()
    # context.browser.find_element_by_xpath("//button[@type='submit']").click()
    # no element


@then(u'new product is in product list')
def step_impl(context):
    # success_tag = context.browser.find_element_by_class("alert alert-success").text
    # assert success_tag == "Success: You have modified products"

    try:
        context.browser.find_element_by_css_selector(".alert")
    except NoSuchElementException:
        return False
    pass



# Scenario: Select and copy product
@given(u'administrator is at products webpage')
def step_impl(context):
    context.browser.get("http://mys01.fit.vutbr.cz:8059/admin/index.php?route=catalog/product/copy&token=" + context.my_token)
    pass


@when(u'admin selects one of the products')
def step_impl(context):
    # context.browser.find_element_by_xpath("//form[@id='form-product']/div/table/tbody/tr/td/input").click()

    x = "//form[@id='form-product']/div/table/tbody/tr[3]/td/input"
    bb = context.browser.find_element_by_xpath(x)
    bb.click()

    pass
    # context.browser.find_element_by_xpath("//input[@name='selected[]']").click()
    pass # no element


@when(u'admin presses copy button')
def step_impl(context):
    context.browser.find_element_by_xpath("//div[@id='content']/div/div/div/button").click()
    pass


@then(u'the same product shows in the list')
def step_impl(context):
    try:
        context.browser.find_element_by_css_selector(".alert")
    except NoSuchElementException:
        return False
    pass


# Scenario: Delete
@given(u'a product is selected')
def step_impl(context):
    x = "http://mys01.fit.vutbr.cz:8059/admin/index.php?route=catalog/product&token="
    context.browser.get(x + context.my_token)
    x = "//form[@id='form-product']/div/table/tbody/tr[3]/td/input"
    bb = context.browser.find_element_by_xpath(x)
    bb.click()


@when(u'admin presses delete button')
def step_impl(context):
    context.browser.find_element_by_xpath("(//button[@type='button'])[2]").click()

    try:
        WebDriverWait(context.browser, 3).until(EC.alert_is_present(),
                                        'Are you sure?')

        alert = context.browser.switch_to.alert
        alert.accept()
        print("alert accepted")
    except TimeoutException:
        assert False


@then(u'the product is not in the list')
def step_impl(context):
    try:
        context.browser.find_element_by_css_selector(".alert")
    except NoSuchElementException:
        return False
    pass


# Scenario: Edit
"""
@given(u'admin is at products webpage')
def step_impl(context):
    context.browser.get("http://mys01.fit.vutbr.cz:8059/admin/index.php?route=catalog/product")
    pass
    #And there is at least one product in the list')
"""
@when(u'admin presses edit button at one of the products')
def step_impl(context):
    x = "//form[@id='form-product']/div/table/tbody/tr/td[8]/a/i"
    element = context.browser.find_element_by_xpath(x)

    element.click()
    pass


@then(u'Edit Product page is loaded')
def step_impl(context):
    t = context.browser.find_element_by_class_name('panel-title').text
    assert t == "Edit Product"
    pass


# Scenario: Edit product, Save
@given(u'admin is at Edit product page')
def step_impl(context):
    context.browser.get("http://mys01.fit.vutbr.cz:8059/admin/index.php?route=catalog/product/edit&token=" + context.my_token + "&product_id=71")
    t = context.browser.find_element_by_class_name('panel-title').text
    print(t)
    assert t == "Edit Product"
    pass


@when(u'admin finishes all changes')
def step_impl(context):
    context.browser.find_element_by_id("input-name1").send_keys("Test name")
    pass


@then(u'product list is shown')
def step_impl(context):
    try:
        context.browser.find_element_by_css_selector(".alert")
    except NoSuchElementException:
        return False
    pass


# Scenario: Filter
# given(u'admin is at products webpage')
@when(u'admin fills in filter details')
def step_impl(context):
    context.product_filter = "Apple Cinema"
    context.browser.find_element_by_xpath("//input[@id='input-name']").send_keys(context.product_filter )
    pass


@when(u'presses the Filter button')
def step_impl(context):
    context.browser.find_element_by_xpath("//form[@id='form-product']/div/table/tbody/tr/td[8]/a")
    pass


@then(u'relevant products are shown in the list')
def step_impl(context):
    val = context.browser.find_element_by_css_selector("tbody .text-left:nth-child(3)")
    val.click()
    css_bb = "tbody > tr:nth-child(1) > .text-left:nth-child(3)"
    filtered_product = context.browser.find_element_by_css_selector(css_bb).text
    print(context.product_filter, filtered_product)
    if context.product_filter not in filtered_product:
        assert False
    pass

# Scenario Outline: Sort
# @when('admin presses ”Product Name”')
@when(u'admin presses "{table_cell}"')
def step_press_sort_category(context, table_cell):
    context.price = 0
    if table_cell == u'Model':
        x = 4
    elif table_cell == u'Price':
        context.price = 1
        x = 5
    elif table_cell == u'Quantity':
        x = 6
    elif table_cell == u'Status':
        x = 7
    else:  # u'Product Name':
        x = 3

    print(x, table_cell)
    xpth_head = "//form[@id='form-product']/div/table/thead/tr/td[" + str(x) + "]/a"
    context.browser.find_element_by_xpath(xpth_head).click()
    time.sleep(1)
    context.list_of_products = []
    for i in range(2, 6):
        xpth = "//form[@id='form-product']/div/table/tbody/tr[" + str(i) + "]/td[" + str(x) + "]"
        if context.browser.find_element_by_xpath is not None:
            item = context.browser.find_element_by_xpath(xpth).text
            if item:
                context.list_of_products.append(item)
        else:
            break
        pass
    pass


@then(u'the list is sorted according to the category')
def step_impl(context):
    print(context.list_of_products)
    if context.price:
        context.list_of_products = sorted(context.list_of_products, key=float)
        print(context.list_of_products.reverse())
        if context.list_of_products.reverse() != context.list_of_products:
            assert False
    else:
        print(sorted(context.list_of_products))
        if sorted(context.list_of_products) != context.list_of_products:
            assert False

    context.list_of_products = []
    context.price = 0


# Scenario Outline: Next page
@when(u'admin presses “<page>” button')
def step_impl(context):
    context.browser.find_element_by_xpath("//a[contains(text(),'2')]")
    pass


@then(u'“<page>” is shown')
def step_impl(context):
    assert context.current_url == "http://mys01.fit.vutbr.cz:8059/admin/index.php?route=catalog/product&token=" \
           + context.my_token + "&sort=pd.name&order=DESC&page=2"
    pass
