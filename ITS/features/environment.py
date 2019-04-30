from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import behave
import time


# Background:
# @given(u'Admin is logged in')
def before_all(context):
    dp = {'browserName': 'firefox', 'marionette': 'true',
        'javascriptEnabled': 'true'}

    context.browser = webdriver.Remote(
        command_executor='http://mys01.fit.vutbr.cz:4444/wd/hub',
        desired_capabilities=dp)
    context.browser.implicitly_wait(15)
    context.base_url = "http://mys01.fit.vutbr.cz:8059/admin/"


def after_all(context):
    #context.browser.quit()
    pass
