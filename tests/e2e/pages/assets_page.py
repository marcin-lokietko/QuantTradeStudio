from selenium.webdriver.common.by import By
from tests.e2e.utils.waiting_webdriver import WaitingWebdriver

class AssetsPage:
    def __init__(self, webdriver):
        self.waiting_webdriver = WaitingWebdriver(webdriver, timeout=15)

    def get_assets(self):
        ''' Returns [{assetSymbolCell, freeQuantityCell, usdtValueCell}] '''
        assets = []

        asset_table_body = self.waiting_webdriver.find_element(By.XPATH, "//tbody")
        asset_table_entries = asset_table_body.find_elements(By.TAG_NAME, "tr")

        for single_entry in asset_table_entries:
            assetSymbolCell = single_entry.find_element(By.CLASS_NAME, "mat-column-assetSymbol")
            freeQuantityCell = single_entry.find_element(By.CLASS_NAME, "mat-column-freeQuantity")
            usdtValueCell = single_entry.find_element(By.CLASS_NAME, "mat-column-usdtValue")

            assets.append({
                "assetSymbolCell": assetSymbolCell,
                "freeQuantityCell": freeQuantityCell,
                "usdtValueCell": usdtValueCell
            })

        return assets

    def get_assets_texts(self):
        ''' Returns [{assetSymbolText, freeQuantityText, usdtValueText}] '''
        assets = self.get_assets()
        assets_texts = []
        for single_entry in assets:
            assets_texts.append({
                "assetSymbolText": single_entry["assetSymbolCell"].text,
                "freeQuantityText": single_entry["freeQuantityCell"].text,
                "usdtValueText": single_entry["usdtValueCell"].text
            })
        return assets_texts

    def open_make_order_dialog(self, assetSymbol):
        assets = self.get_assets()
        for single_entry in assets:
            if assetSymbol in single_entry["assetSymbolCell"].text:
                single_entry["assetSymbolCell"].find_element(By.XPATH, ".//button").click()
                self.waiting_webdriver.find_element(By.ID, "make-order-dialog-menu-item").click()
                return
        else:
            raise Exception(f"Asset {assetSymbol} not found")
