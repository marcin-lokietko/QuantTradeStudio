import { Injectable } from '@angular/core';
import { environment } from '@env/environment';

export enum RequestResult {
  Success = 'Success',
  Fail = 'Fail',
}

export interface Order {
  orderId: number;
  assetPair: string;
  origQuantity: string;
  executedQuantity: string;
  orderSide: string;
  price: string;
}

export interface Balance {
  assetSymbol: string;
  freeQuantity: string;
  usdtValue: string;
}

export interface AvailableQuoteAsset {
  baseAssetUnitPrice: string;
  quoteAsset: string;
}

@Injectable({
  providedIn: 'root',
})
export class DataService {
  async getOwnedAssets(): Promise<Balance[] | undefined> {
    console.log('DataService.getOwnedAssets');
    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + '/assets');
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      console.log('GET /assets response:', JSON.stringify(data));
      return data as Balance[];
    } catch (error) {
      console.error('GET /assets error:', error);
      return undefined;
    }
  }

  async getOpenOrders(): Promise<Order[] | undefined> {
    console.log('DataService.getOpenOrders');
    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + '/openOrders');
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      console.log('GET /openOrders response:', JSON.stringify(data));
      return data as Order[];
    } catch (error) {
      console.error('GET /openOrders error:', error);
      return undefined;
    }
  }

  async getAvailableBaseAssets(quoteAsset: string | undefined = undefined): Promise<string[] | undefined> {
    console.log('DataService.getAvailableBaseAssets quoteAsset=' + quoteAsset);

    let urlWithQuery = '/availableBaseAssets';
    if (quoteAsset !== undefined) {
      const params = new URLSearchParams({
        quoteAsset,
      });
      urlWithQuery += `?${params.toString()}`;
    }

    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + urlWithQuery);
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      console.log('GET' + urlWithQuery + ' response:', JSON.stringify(data));

      return data.map((elem: any) => {
        return elem.assetSymbol;
      });
    } catch (error) {
      console.error('GET' + urlWithQuery + ' error:', error);
      return undefined;
    }
  }

  async getAvailableQuoteAssets(selectedBaseAsset: string): Promise<AvailableQuoteAsset[] | undefined> {
    console.log('DataService.getAvailableQuoteAssets');
    const params = new URLSearchParams({
      baseAsset: selectedBaseAsset,
    });
    const urlWithQuery = `/availableQuoteAssets?${params.toString()}`;
    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + urlWithQuery);
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      console.log('GET' + urlWithQuery + ' response:', JSON.stringify(data));
      return data as AvailableQuoteAsset[];
    } catch (error) {
      console.error('GET' + urlWithQuery + ' error:', error);
      return undefined;
    }
  }

  async makeOrder(
    selectedBaseAsset: string,
    selectedQuoteAsset: string,
    orderSide: string,
    baseAssetAmount: string,
  ): Promise<RequestResult> {
    console.log(
      `DataService.makeOrder selectedBaseAsset=${selectedBaseAsset}, selectedQuoteAsset=${selectedQuoteAsset}, orderSide=${orderSide}, baseAssetAmount=${baseAssetAmount}`,
    );

    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + '/makeOrder', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Access-Control-Allow-Headers': '*',
        },
        body: JSON.stringify({ selectedBaseAsset, selectedQuoteAsset, orderSide, baseAssetAmount }),
      });
      if (!response.ok) {
        return RequestResult.Fail;
      }
      console.info('POST /makeOrder success:');
      return RequestResult.Success;
    } catch (error) {
      console.error('POST /makeOrder error:', error);
      return RequestResult.Fail;
    }
  }

  async startBot(botParams: any): Promise<RequestResult> {
    console.log('DataService.startBot, params:', JSON.stringify(botParams));

    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + '/startBot', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Access-Control-Allow-Headers': '*',
        },
        body: JSON.stringify(botParams),
      });
      if (!response.ok) {
        return RequestResult.Fail;
      }
      console.info('POST /startBot success:');
      return RequestResult.Success;
    } catch (error) {
      console.error('POST /startBot error:', error);
      return RequestResult.Fail;
    }
  }

  async getQuoteAssetsSuitableForRebalancing(): Promise<string[] | undefined> {
    console.log('DataService.getQuoteAssetsSuitableForRebalancing');
    try {
      const response = await fetch(environment.algoTraderBackendUrlPrefix + '/quoteAssetsSuitableForRebalancing');
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const data = await response.json();
      console.log('GET /quoteAssetsSuitableForRebalancing response:', JSON.stringify(data));

      return data.map((elem: any) => {
        return elem.assetSymbol;
      });
    } catch (error) {
      console.error('GET /quoteAssetsSuitableForRebalancing error:', error);
      return undefined;
    }
  }
}
