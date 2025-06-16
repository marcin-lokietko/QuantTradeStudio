import { ComponentFixture, TestBed } from '@angular/core/testing';
import { AssetsPage } from './assets-page.component';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { Balance, DataService, RequestResult } from '@app/services/data.service';
import { NotificationService } from '@app/services/notification.service';
import { MatSortModule } from '@angular/material/sort';
import { MatTableModule } from '@angular/material/table';
import { MatMenuModule } from '@angular/material/menu';
import { MatIconModule } from '@angular/material/icon';
import { MatDialog } from '@angular/material/dialog';

describe('AssetsPage', () => {
  let component: AssetsPage;
  let fixture: ComponentFixture<AssetsPage>;

  let mockDataService: jasmine.SpyObj<DataService>;
  let mockNotificationService: jasmine.SpyObj<NotificationService>;

  const mockBalances: Balance[] = [
    { assetSymbol: 'BTC', freeQuantity: '0.5', usdtValue: '25000' },
    { assetSymbol: 'ETH', freeQuantity: '2', usdtValue: '6000' },
  ];

  beforeEach(async () => {
    mockDataService = jasmine.createSpyObj('DataService', ['getOwnedAssets', 'makeOrder']);
    mockNotificationService = jasmine.createSpyObj('NotificationService', ['show']);

    await TestBed.configureTestingModule({
      declarations: [AssetsPage],
      imports: [MatProgressSpinnerModule, MatTableModule, MatSortModule, MatMenuModule, MatIconModule],
      providers: [
        { provide: DataService, useValue: mockDataService },
        { provide: NotificationService, useValue: mockNotificationService },
      ],
    }).compileComponents();

    fixture = TestBed.createComponent(AssetsPage);
    component = fixture.componentInstance;
  });

  async function triggerComponentInitAndStabilization() {
    fixture.detectChanges(); // ngOnInit
    await fixture.whenStable(); // wait for async operations to complete
    fixture.detectChanges(); // re-render after async operations
  }

  afterEach(() => {
    mockDataService.getOwnedAssets.calls.reset();
    mockDataService.makeOrder.calls.reset();
    mockNotificationService.show.calls.reset();
  });

  it('should create the component', async () => {
    mockDataService.getOwnedAssets.and.returnValue(Promise.resolve([]));
    mockDataService.makeOrder.and.returnValue(Promise.resolve(RequestResult.Success));
    mockNotificationService.show.and.callFake(() => {});

    expect(component.isLoading).toBeTrue();

    await triggerComponentInitAndStabilization();

    expect(component).toBeTruthy();
  });

  it('should call makeOrder after MakeOrderDialog is closed with a result', () => {
    const mockDialogResult = {
      selectedBaseAsset: 'BTC',
      selectedQuoteAsset: 'USDT',
      orderSide: 'BUY',
      baseAssetAmount: '1',
    };
    const afterClosedSpy = jasmine.createSpyObj('afterClosed', ['subscribe']);
    afterClosedSpy.subscribe.and.callFake((fn: (result: any) => void) => fn(mockDialogResult));
    const dialogRefSpy = { afterClosed: () => afterClosedSpy };
    const dialogSpy = spyOn(TestBed.inject(MatDialog), 'open').and.returnValue(dialogRefSpy as any);
    const makeOrderSpy = spyOn(component, 'makeOrder');

    component.showMakeOrderDialog({ assetSymbol: 'BTC' });

    expect(dialogSpy).toHaveBeenCalled();
    expect(makeOrderSpy).toHaveBeenCalledWith('BTC', 'USDT', 'BUY', '1');
  });

  it('should call makeOrder after MakeOrderDialog is closed with a result', () => {
    const mockDialogResult = {
      selectedBaseAsset: 'BTC',
      selectedQuoteAsset: 'USDT',
      orderSide: 'BUY',
      baseAssetAmount: '1',
    };
    const afterClosedSpy = jasmine.createSpyObj('afterClosed', ['subscribe']);
    afterClosedSpy.subscribe.and.callFake((fn: (result: any) => void) => fn(mockDialogResult));
    const dialogRefSpy = { afterClosed: () => afterClosedSpy };
    const dialogSpy = spyOn(TestBed.inject(MatDialog), 'open').and.returnValue(dialogRefSpy as any);
    mockDataService.makeOrder.and.returnValue(Promise.resolve(RequestResult.Success));

    component.showMakeOrderDialog({ assetSymbol: 'BTC' });

    expect(dialogSpy).toHaveBeenCalled();
    expect(mockDataService.makeOrder).toHaveBeenCalledWith('BTC', 'USDT', 'BUY', '1');
  });

  describe('when the owned assets are fetched', () => {
    beforeEach(async () => {
      mockDataService.getOwnedAssets.and.returnValue(Promise.resolve(mockBalances));
      mockDataService.makeOrder.and.returnValue(Promise.resolve(RequestResult.Success));
      mockNotificationService.show.and.callFake(() => {});

      await triggerComponentInitAndStabilization();
    });

    it('should store the owned assets', async () => {
      expect(component.balances).toEqual(mockBalances);
      expect(component.dataSource.data).toEqual(mockBalances);
      expect(component.isLoading).toBeFalse();
    });

    it('should present the owned assets', async () => {
      const rows = fixture.nativeElement.querySelectorAll('tr');
      expect(rows.length).toBe(mockBalances.length + 1);

      const headerRow = rows[0].querySelectorAll('th');
      expect(headerRow[0].textContent).toContain('AssetSymbol');
      expect(headerRow[1].textContent).toContain('Free');
      expect(headerRow[2].textContent).toContain('USDT value');

      const firstDataRow = rows[1].querySelectorAll('td');
      expect(firstDataRow[0].textContent).toContain('BTC');
      expect(firstDataRow[1].textContent).toContain('0.5');
      expect(firstDataRow[2].textContent).toContain('25000');

      const secondDataRow = rows[2].querySelectorAll('td');
      expect(secondDataRow[0].textContent).toContain('ETH');
      expect(secondDataRow[1].textContent).toContain('2');
      expect(secondDataRow[2].textContent).toContain('6000');
    });
  });
});
