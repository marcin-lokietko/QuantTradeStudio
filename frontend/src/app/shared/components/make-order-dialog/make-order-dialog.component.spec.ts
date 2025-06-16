import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { AvailableQuoteAsset, DataService } from '@app/services/data.service';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';
import { MakeOrderDialog } from './make-order-dialog.component';
import { MatButtonModule } from '@angular/material/button';
import { MAT_DIALOG_DATA, MatDialogModule, MatDialogRef } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatMenuModule } from '@angular/material/menu';
import { MatRadioModule } from '@angular/material/radio';
import { MatSelectModule } from '@angular/material/select';
import { ChangeDetectorRef } from '@angular/core';

describe('MakeOrderDialog', () => {
  let component: MakeOrderDialog;
  let fixture: ComponentFixture<MakeOrderDialog>;

  let mockDataService: jasmine.SpyObj<DataService>;
  let mockDialogRef: jasmine.SpyObj<MatDialogRef<MakeOrderDialog>>;

  beforeEach(async () => {
    mockDataService = jasmine.createSpyObj('DataService', ['getAvailableBaseAssets', 'getAvailableQuoteAssets']);
    mockDialogRef = jasmine.createSpyObj('MatDialogRef', ['close']);

    await TestBed.configureTestingModule({
      declarations: [],
      imports: [
        MakeOrderDialog,
        MatButtonModule,
        MatDialogModule,
        MatFormFieldModule,
        MatIconModule,
        MatInputModule,
        MatMenuModule,
        MatProgressSpinnerModule,
        MatRadioModule,
        MatSelectModule,
        MatSortModule,
        MatTableModule,
      ],
      providers: [
        { provide: DataService, useValue: mockDataService },
        { provide: MAT_DIALOG_DATA, useValue: { initialAssetToBuy: 'BTC' } },
        { provide: MatDialogRef, useValue: mockDialogRef },
        { provide: ChangeDetectorRef, useValue: { markForCheck: () => {} } },
      ],
    }).compileComponents();

    fixture = TestBed.createComponent(MakeOrderDialog);
    component = fixture.componentInstance;
  });

  afterEach(() => {
    mockDataService.getAvailableBaseAssets.calls.reset();
    mockDataService.getAvailableQuoteAssets.calls.reset();
  });

  async function triggerComponentInitAndStabilization() {
    fixture.detectChanges(); // ngOnInit
    await fixture.whenStable(); // wait for async operations to complete
    fixture.detectChanges(); // re-render after async operations
  }

  describe('when no assets are fetched', () => {
    beforeEach(async () => {
      mockDataService.getAvailableBaseAssets.and.returnValue(Promise.resolve(undefined));
      mockDataService.getAvailableQuoteAssets.and.returnValue(Promise.resolve(undefined));
      await triggerComponentInitAndStabilization();
    });

    it('should create the component', async () => {
      expect(component).toBeTruthy();
    });

    it('should call dialogRef.close with undefined on cancel()', async () => {
      component.cancel();
      expect(mockDialogRef.close).toHaveBeenCalledWith(undefined);
    });

    it('should call dialogRef.close with result on closeWithResult()', async () => {
      component.selectedBaseAsset = 'BTC';
      component.selectedQuoteAsset = 'USDT';
      component.orderSide = 'BUY';
      component.baseAssetAmount = '1.5';

      component.closeWithResult();

      expect(mockDialogRef.close).toHaveBeenCalledWith({
        selectedBaseAsset: 'BTC',
        selectedQuoteAsset: 'USDT',
        orderSide: 'BUY',
        baseAssetAmount: '1.5',
      });
    });

    it('isInputValid should return true only if all fields are set', async () => {
      component.selectedBaseAsset = 'BTC';
      component.selectedQuoteAsset = 'USDT';
      component.baseAssetAmount = '1.5';
      expect(component.isInputValid()).toBeTrue();

      component.baseAssetAmount = '';
      expect(component.isInputValid()).toBeFalse();
    });

    it('should compute presented texts correctly', () => {
      component.selectedBaseAsset = 'BTC';
      component.selectedQuoteAsset = 'USDT';
      component.orderSide = 'BUY';
      component.baseAssetAmount = '1.5';

      expect(component.title).toContain('Trade BTC/USDT');
      expect(component.subtitle).toBe('Spot market order');
      expect(component.details).toContain('You are buying 1.5 BTC for USDT');
    });
  });

  describe('when the available assets are fetched', () => {
    beforeEach(async () => {
      const mockAvailableBaseAssets = ['BTC', 'ETH', 'XRP'];
      mockDataService.getAvailableBaseAssets.and.returnValue(Promise.resolve(mockAvailableBaseAssets));

      const mockAvailableQuoteAssets: AvailableQuoteAsset[] = [
        { quoteAsset: 'USDT', baseAssetUnitPrice: '80000' },
        { quoteAsset: 'EUR', baseAssetUnitPrice: '90000' },
      ];
      mockDataService.getAvailableQuoteAssets.and.returnValue(Promise.resolve(mockAvailableQuoteAssets));

      await triggerComponentInitAndStabilization();
    });

    it('should store the available assets', async () => {
      expect(component.availableBaseAssets).toEqual(['BTC', 'ETH', 'XRP']);
      expect(component.availableQuoteAssets).toEqual(['USDT', 'EUR']);
    });

    it('should show/hide quote asset and order side selection', () => {
      component.selectedBaseAsset = '';
      expect(component.isQuoteAssetSelectionVisible).toBeFalse();

      component.selectedBaseAsset = 'BTC';
      expect(component.isQuoteAssetSelectionVisible).toBeTrue();

      component.selectedQuoteAsset = '';
      expect(component.isOrderSideSelectionVisible).toBeFalse();

      component.selectedQuoteAsset = 'USDT';
      expect(component.isOrderSideSelectionVisible).toBeTrue();
    });
  });
});
