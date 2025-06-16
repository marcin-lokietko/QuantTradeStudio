import { TestBed } from '@angular/core/testing';
import { DataService, RequestResult } from './data.service';

describe('DataService', () => {
  let service: DataService;

  beforeEach(() => {
    (window as any).fetch = () => {};
    TestBed.configureTestingModule({});
    service = TestBed.inject(DataService);
  });

  afterEach(() => {
    (window as any).fetch = undefined;
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });

  describe('getOwnedAssets', () => {
    it('should return balances on success', async () => {
      const mockBalances = [{ assetSymbol: 'BTC', freeQuantity: '1', usdtValue: '30000' }];
      spyOn(window, 'fetch').and.resolveTo(new Response(JSON.stringify(mockBalances), { status: 200 }));

      const result = await service.getOwnedAssets();
      expect(result).toEqual(mockBalances);
    });

    it('should return undefined on fetch error', async () => {
      spyOn(window, 'fetch').and.rejectWith(new Error('Network error'));
      const result = await service.getOwnedAssets();
      expect(result).toBeUndefined();
    });

    it('should return undefined on non-OK response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 500 }));
      const result = await service.getOwnedAssets();
      expect(result).toBeUndefined();
    });
  });

  describe('getOpenOrders', () => {
    it('should return orders on success', async () => {
      const mockOrders = [{ orderId: 1, assetPair: 'BTCUSD', origQuantity: '1', executedQuantity: '0', orderSide: 'BUY', price: '30000' }];
      spyOn(window, 'fetch').and.resolveTo(new Response(JSON.stringify(mockOrders), { status: 200 }));

      const result = await service.getOpenOrders();
      expect(result).toEqual(mockOrders);
    });

    it('should return undefined on fetch error', async () => {
      spyOn(window, 'fetch').and.rejectWith(new Error('Network error'));
      const result = await service.getOpenOrders();
      expect(result).toBeUndefined();
    });

    it('should return undefined on non-OK response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 500 }));
      const result = await service.getOpenOrders();
      expect(result).toBeUndefined();
    });
  });

  describe('getAvailableBaseAssets', () => {
    it('should return asset symbols on success', async () => {
      const mockResponse = [{ assetSymbol: 'BTC' }, { assetSymbol: 'ETH' }];
      spyOn(window, 'fetch').and.resolveTo(new Response(JSON.stringify(mockResponse), { status: 200 }));

      const result = await service.getAvailableBaseAssets();
      expect(result).toEqual(['BTC', 'ETH']);
    });

    it('should return undefined on fetch error', async () => {
      spyOn(window, 'fetch').and.rejectWith(new Error('Network error'));
      const result = await service.getAvailableBaseAssets();
      expect(result).toBeUndefined();
    });

    it('should return undefined on non-OK response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 500 }));
      const result = await service.getAvailableBaseAssets();
      expect(result).toBeUndefined();
    });
  });

  describe('getAvailableQuoteAssets', () => {
    it('should return quote assets on success', async () => {
      const mockResponse = [{ quoteAsset: 'USDT', baseAssetUnitPrice: '1' }];
      spyOn(window, 'fetch').and.resolveTo(new Response(JSON.stringify(mockResponse), { status: 200 }));

      const result = await service.getAvailableQuoteAssets('BTC');
      expect(result).toEqual(mockResponse);
    });

    it('should return undefined on fetch error', async () => {
      spyOn(window, 'fetch').and.rejectWith(new Error('Network error'));
      const result = await service.getAvailableQuoteAssets('BTC');
      expect(result).toBeUndefined();
    });

    it('should return undefined on non-OK response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 500 }));
      const result = await service.getAvailableQuoteAssets('BTC');
      expect(result).toBeUndefined();
    });
  });

  describe('makeOrder', () => {
    it('should return Success on 200 response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 200 }));

      const result = await service.makeOrder('BTC', 'USDT', 'BUY', '1');
      expect(result).toBe(RequestResult.Success);
    });

    it('should return Fail on non-OK response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 400 }));

      const result = await service.makeOrder('BTC', 'USDT', 'BUY', '1');
      expect(result).toBe(RequestResult.Fail);
    });

    it('should return Fail on fetch error', async () => {
      spyOn(window, 'fetch').and.rejectWith(new Error('Network error'));

      const result = await service.makeOrder('BTC', 'USDT', 'BUY', '1');
      expect(result).toBe(RequestResult.Fail);
    });
  });

  describe('startBot', () => {
    it('should return Success on 200 response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 200 }));

      const result = await service.startBot({ foo: 'bar' });
      expect(result).toBe(RequestResult.Success);
    });

    it('should return Fail on non-OK response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 400 }));

      const result = await service.startBot({ foo: 'bar' });
      expect(result).toBe(RequestResult.Fail);
    });

    it('should return Fail on fetch error', async () => {
      spyOn(window, 'fetch').and.rejectWith(new Error('Network error'));

      const result = await service.startBot({ foo: 'bar' });
      expect(result).toBe(RequestResult.Fail);
    });
  });

  describe('getQuoteAssetsSuitableForRebalancing', () => {
    it('should return asset symbols on success', async () => {
      const mockResponse = [{ assetSymbol: 'USDT' }, { assetSymbol: 'EUR' }];
      spyOn(window, 'fetch').and.resolveTo(new Response(JSON.stringify(mockResponse), { status: 200 }));

      const result = await service.getQuoteAssetsSuitableForRebalancing();
      expect(result).toEqual(['USDT', 'EUR']);
    });

    it('should return undefined on fetch error', async () => {
      spyOn(window, 'fetch').and.rejectWith(new Error('Network error'));
      const result = await service.getQuoteAssetsSuitableForRebalancing();
      expect(result).toBeUndefined();
    });

    it('should return undefined on non-OK response', async () => {
      spyOn(window, 'fetch').and.resolveTo(new Response('', { status: 500 }));
      const result = await service.getQuoteAssetsSuitableForRebalancing();
      expect(result).toBeUndefined();
    });
  });
});
