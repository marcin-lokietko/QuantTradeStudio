import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';
import { MatButtonModule } from '@angular/material/button';
import { MAT_DIALOG_DATA, MatDialog, MatDialogModule, MatDialogRef } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatMenuModule } from '@angular/material/menu';
import { MatRadioModule } from '@angular/material/radio';
import { MatSelectModule } from '@angular/material/select';
import { ChangeDetectorRef } from '@angular/core';
import { BacktestDialog } from './backtest-dialog.component';
import { MatNativeDateModule } from '@angular/material/core';
import { MatDatepickerModule } from '@angular/material/datepicker';
import { NotificationService } from '@app/services/notification.service';
import { DataService, RequestResult } from '@app/services/data.service';

describe('BacktestDialog', () => {
  let component: BacktestDialog;
  let fixture: ComponentFixture<BacktestDialog>;
  let mockDialogRef: jasmine.SpyObj<MatDialogRef<BacktestDialog>>;

  const mockDialogData = {
    botParams: {
      botName: 'Rebalancer',
      quoteAsset: 'USDT',
    },
    availableInitialAssets: ['BTC', 'ETH'],
  };

  let mockDataService: jasmine.SpyObj<DataService>;
  let mockDialog: jasmine.SpyObj<MatDialog>;
  let mockNotificationService: jasmine.SpyObj<NotificationService>;

  beforeEach(async () => {
    mockDialogRef = jasmine.createSpyObj('MatDialogRef', ['close']);
    mockDataService = jasmine.createSpyObj('DataService', ['testBot']);
    mockDialog = jasmine.createSpyObj('MatDialog', ['open']);
    mockNotificationService = jasmine.createSpyObj('NotificationService', ['show']);

    await TestBed.configureTestingModule({
      declarations: [],
      imports: [
        BacktestDialog,
        MatButtonModule,
        MatButtonModule,
        MatDialogModule,
        MatFormFieldModule,
        MatIconModule,
        MatInputModule,
        MatMenuModule,
        MatProgressSpinnerModule,
        MatRadioModule,
        MatSelectModule,
        MatDatepickerModule,
        MatNativeDateModule,
        MatTableModule,
      ],
      providers: [
        { provide: MAT_DIALOG_DATA, useValue: mockDialogData },
        { provide: MatDialogRef, useValue: mockDialogRef },
        { provide: ChangeDetectorRef, useValue: { markForCheck: () => {} } },
        { provide: DataService, useValue: mockDataService },
        { provide: MatDialog, useValue: mockDialog },
        { provide: NotificationService, useValue: mockNotificationService },
      ],
    }).compileComponents();

    fixture = TestBed.createComponent(BacktestDialog);
    component = fixture.componentInstance;
  });

  async function triggerComponentInitAndStabilization() {
    fixture.detectChanges(); // ngOnInit
    await fixture.whenStable(); // wait for async operations to complete
    fixture.detectChanges(); // re-render after async operations
  }

  async function setValidBacktestConfiguration() {
    await triggerComponentInitAndStabilization();

    component.transactionFeePercent = '2';

    component.simulationStartDate = new Date('2025-06-01');
    component.simulationStartTime = '11:00';

    component.simulationEndDate = new Date('2025-07-01');
    component.simulationEndTime = '13:00';

    component.initialOwnedAssets = [
      { assetSymbol: 'BTC', freeQuantity: '2' },
      { assetSymbol: 'ETH', freeQuantity: '80' },
    ];

    component.botParams = mockDialogData.botParams;
    component.updateUnixTimestamps();
    await triggerComponentInitAndStabilization();
  }

  function expectButtonNotRendered() {
    const compiled = fixture.nativeElement;
    const launchButton = compiled.querySelector('[data-testid="backtest-dialog-launch-button"]');

    expect(launchButton).toBeFalsy();
    expect(component.isConfigValid).toBeFalsy();
  }

  afterEach(() => {
    mockDataService.testBot.calls.reset();
    mockNotificationService.show.calls.reset();
  });

  it('should create the component', async () => {
    expect(component).toBeTruthy();
  });

  it('should not enable backtest launch when transactionFeePercent is negative', async () => {
    await setValidBacktestConfiguration();
    component.transactionFeePercent = '-5';
    fixture.detectChanges();

    expectButtonNotRendered();
  });

  it('should not enable backtest launch when initialOwnedAssets is empty', async () => {
    await setValidBacktestConfiguration();
    component.initialOwnedAssets = [];
    fixture.detectChanges();

    expectButtonNotRendered();
  });

  it('should not enable backtest launch when start time is after end time', async () => {
    await setValidBacktestConfiguration();
    component.simulationStartDate = new Date('2025-07-01');
    component.simulationStartTime = '12:00';
    component.simulationEndDate = new Date('2025-07-01');
    component.simulationEndTime = '11:00';
    component.updateUnixTimestamps();
    fixture.detectChanges();

    expectButtonNotRendered();
  });

  describe('when backtest configuration is correct', async () => {
    beforeEach(async () => {
      await setValidBacktestConfiguration();
    });

    it('should call testBot and show notification on failure', async () => {
      mockDataService.testBot.and.returnValue(Promise.resolve(undefined));
      await component.launchBacktest();

      expect(mockDataService.testBot).toHaveBeenCalled();
      expect(mockNotificationService.show).toHaveBeenCalledWith('Failed to launch the backtest', 3000, jasmine.anything());
    });

    it('should call testBot and show results on success', async () => {
      const backtestResults = {
        assetHistoryIfHeld: [],
        botAssetHistory: [],
        totalProfitOrLossInAbsolute: '1000',
        totalProfitOrLossInAbsoluteIfHeld: '900',
        totalProfitOrLossInPercent: 10,
        totalProfitOrLossInPercentIfHeld: 9,
        absoluteAsset: 'USDT',
      };
      mockDataService.testBot.and.returnValue(Promise.resolve(backtestResults));
      await component.launchBacktest();
      expect(mockDataService.testBot).toHaveBeenCalledWith(
        mockDialogData.botParams,
        jasmine.objectContaining({
          transactionFeePercent: 2,
          //Date is local time, but getTime() gives UTC offset
          //Month is 0-based
          simulationStart: new Date(2025, 5, 1, 11, 0, 0).getTime(),
          simulationEnd: new Date(2025, 6, 1, 13, 0, 0).getTime(),
          initialOwnedAssets: [
            { assetSymbol: 'BTC', freeQuantity: '2' },
            { assetSymbol: 'ETH', freeQuantity: '80' },
          ],
        }),
      );
      expect(mockNotificationService.show).not.toHaveBeenCalled();

      fixture.detectChanges();

      const compiled = fixture.nativeElement;
      const totalProfitIfHeld = compiled.querySelector('[data-testid="backtest-results-total-profit-if-held"]');
      const totalProfitWithTrades = compiled.querySelector('[data-testid="backtest-results-total-profit-with-trades"]');

      expect(totalProfitIfHeld).toBeTruthy();
      expect(totalProfitIfHeld.textContent).toContain('900');

      expect(totalProfitWithTrades).toBeTruthy();
      expect(totalProfitWithTrades.textContent).toContain('1,000');
    });
  });
});
