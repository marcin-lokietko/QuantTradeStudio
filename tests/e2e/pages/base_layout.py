from selenium.webdriver.common.by import By

class BaseLayout:
    def __init__(self, webdriver):
        self.webdriver = webdriver

    def navigate_to_assets_page(self):
        main_menu = self.webdriver.find_element(By.ID, "main-menu")
        main_menu.click()

        assets_menu_item = self.webdriver.find_element(By.ID, "assets-menu-item")
        assets_menu_item.click()
