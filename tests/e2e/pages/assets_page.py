from selenium.webdriver.common.by import By

class AssetsPage:
    def __init__(self, webdriver):
        self.webdriver = webdriver

    def get_assets(self):
        ''' Returns [{assetSymbolText, freeQuantityText, usdtValueText}] '''
        assets = []

        asset_table_body = self.webdriver.find_element(By.XPATH, "//tbody")
        asset_table_entries = asset_table_body.find_elements(By.TAG_NAME, "tr")

        for single_entry in asset_table_entries:
            assetSymbolCell = single_entry.find_element(By.CLASS_NAME, "mat-column-assetSymbol")
            freeQuantityCell = single_entry.find_element(By.CLASS_NAME, "mat-column-freeQuantity")
            usdtValueCell = single_entry.find_element(By.CLASS_NAME, "mat-column-usdtValue")

            assets.append({
                "assetSymbolText": assetSymbolCell.text,
                "freeQuantityText": freeQuantityCell.text,
                "usdtValueText": usdtValueCell.text
            })

        return assets
