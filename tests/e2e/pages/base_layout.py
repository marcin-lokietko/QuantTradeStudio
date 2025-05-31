from selenium.webdriver.common.by import By
from tests.e2e.utils.waiting_webdriver import WaitingWebdriver

class BaseLayout:
    def __init__(self, webdriver):
        self.waiting_webdriver = WaitingWebdriver(webdriver, timeout=15)

    def navigate_to_assets_page(self):
        self.waiting_webdriver.find_element(By.ID, "main-menu").click()
        self.waiting_webdriver.find_element(By.ID, "assets-menu-item").click()

    def navigate_to_orders_page(self):
        self.waiting_webdriver.find_element(By.ID, "main-menu").click()
        self.waiting_webdriver.find_element(By.ID, "orders-menu-item").click()

    def navigate_to_bots_page(self):
        self.waiting_webdriver.find_element(By.ID, "main-menu").click()
        self.waiting_webdriver.find_element(By.ID, "bots-menu-item").click()
