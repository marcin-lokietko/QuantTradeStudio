import { ComponentFixture, TestBed } from '@angular/core/testing';
import { OrdersPage } from './orders-page.component';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { DataService, Order } from '@app/services/data.service';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';

describe('OrdersPage', () => {
  let component: OrdersPage;
  let fixture: ComponentFixture<OrdersPage>;

  let mockDataService: jasmine.SpyObj<DataService>;

  const mockOrders: Order[] = [
    { orderId: 1, assetPair: 'BTCUSD', orderSide: 'BUY', price: '100000', origQuantity: '1', executedQuantity: '0' },
    { orderId: 1, assetPair: 'ETHUSD', orderSide: 'SELL', price: '3000', origQuantity: '2', executedQuantity: '1' },
  ];

  beforeEach(async () => {
    mockDataService = jasmine.createSpyObj('DataService', ['getOpenOrders']);

    await TestBed.configureTestingModule({
      declarations: [OrdersPage],
      imports: [MatProgressSpinnerModule, MatTableModule, MatSortModule],
      providers: [{ provide: DataService, useValue: mockDataService }],
    }).compileComponents();

    fixture = TestBed.createComponent(OrdersPage);
    component = fixture.componentInstance;
  });

  afterEach(() => {
    mockDataService.getOpenOrders.calls.reset();
  });

  it('should create the component', () => {
    mockDataService.getOpenOrders.and.returnValue(Promise.resolve(undefined));
    fixture.detectChanges(); // triggers ngOnInit

    expect(component).toBeTruthy();
  });

  describe('when the active orders are fetched', () => {
    beforeEach(async () => {
      mockDataService.getOpenOrders.and.returnValue(Promise.resolve(mockOrders));

      // trigger ngOnInit
      fixture.detectChanges();

      // wait for async operation to complete and render the component
      await fixture.whenStable();
      fixture.detectChanges();
    });

    it('should store the active orders', async () => {
      expect(component.orders).toEqual(mockOrders);
      expect(component.dataSource.data).toEqual(mockOrders);
      expect(component.isLoading).toBeFalse();
    });

    it('should present the active orders', async () => {
      const rows = fixture.nativeElement.querySelectorAll('tr');
      expect(rows.length).toBe(mockOrders.length + 1);

      const headerRow = rows[0].querySelectorAll('th');
      expect(headerRow[0].textContent).toContain('Asset Pair');
      expect(headerRow[1].textContent).toContain('Order Side');
      expect(headerRow[2].textContent).toContain('Price');
      expect(headerRow[3].textContent).toContain('Orig Quantity');
      expect(headerRow[4].textContent).toContain('Executed Quantity');

      const firstDataRow = rows[1].querySelectorAll('td');
      expect(firstDataRow[0].textContent).toContain('BTCUSD');
      expect(firstDataRow[1].textContent).toContain('BUY');
      expect(firstDataRow[2].textContent).toContain('100000');
      expect(firstDataRow[3].textContent).toContain('1');
      expect(firstDataRow[4].textContent).toContain('0');

      const secondDataRow = rows[2].querySelectorAll('td');
      expect(secondDataRow[0].textContent).toContain('ETHUSD');
      expect(secondDataRow[1].textContent).toContain('SELL');
      expect(secondDataRow[2].textContent).toContain('3000');
      expect(secondDataRow[3].textContent).toContain('2');
      expect(secondDataRow[4].textContent).toContain('1');
    });
  });
});
