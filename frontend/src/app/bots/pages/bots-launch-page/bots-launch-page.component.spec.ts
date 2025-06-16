import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { DataService, Order, RequestResult } from '@app/services/data.service';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';
import { BotsLaunchPage } from './bots-launch-page.component';
import { MatSelectModule } from '@angular/material/select';
import { MatMenuModule } from '@angular/material/menu';
import { MatInputModule } from '@angular/material/input';
import { MatIconModule } from '@angular/material/icon';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatButtonModule } from '@angular/material/button';
import { MatDialog } from '@angular/material/dialog';
import { NotificationService } from '@app/services/notification.service';
import { FormsModule } from '@angular/forms';

describe('BotsLaunchPage', () => {
  let component: BotsLaunchPage;
  let fixture: ComponentFixture<BotsLaunchPage>;

  let mockDataService: jasmine.SpyObj<DataService>;
  let mockDialog: jasmine.SpyObj<MatDialog>;
  let mockNotificationService: jasmine.SpyObj<NotificationService>;

  beforeEach(async () => {
    mockDataService = jasmine.createSpyObj('DataService', [
      'getOpenOrders',
      'getQuoteAssetsSuitableForRebalancing',
      'getAvailableBaseAssets',
      'startBot',
    ]);
    mockDialog = jasmine.createSpyObj('MatDialog', ['open']);
    mockNotificationService = jasmine.createSpyObj('NotificationService', ['show']);

    await TestBed.configureTestingModule({
      declarations: [BotsLaunchPage],
      imports: [
        FormsModule,
        MatButtonModule,
        MatCheckboxModule,
        MatFormFieldModule,
        MatIconModule,
        MatInputModule,
        MatMenuModule,
        MatProgressSpinnerModule,
        MatSelectModule,
        MatSortModule,
        MatTableModule,
      ],
      providers: [
        { provide: DataService, useValue: mockDataService },
        { provide: MatDialog, useValue: mockDialog },
        { provide: NotificationService, useValue: mockNotificationService },
      ],
    }).compileComponents();

    fixture = TestBed.createComponent(BotsLaunchPage);
    component = fixture.componentInstance;
  });

  async function triggerComponentInitAndStabilization() {
    fixture.detectChanges(); // ngOnInit
    await fixture.whenStable(); // wait for async operations to complete
    fixture.detectChanges(); // re-render after async operations
  }

  afterEach(() => {
    mockDataService.getOpenOrders.calls.reset();
    mockDataService.getQuoteAssetsSuitableForRebalancing.calls.reset();
    mockDataService.getAvailableBaseAssets.calls.reset();
    mockDataService.startBot.calls.reset();
    mockNotificationService.show.calls.reset();
  });

  it('should create the component', async () => {
    await triggerComponentInitAndStabilization();
    expect(component).toBeTruthy();
  });

  describe('when Rebalancer bot is selected', async () => {
    beforeEach(async () => {
      mockDataService.getQuoteAssetsSuitableForRebalancing.and.returnValue(Promise.resolve(['USDT', 'EUR']));
      mockDataService.getAvailableBaseAssets.and.returnValue(Promise.resolve(['BTC', 'ETH']));

      component.selectBot('Rebalancer');
      await triggerComponentInitAndStabilization();
      expect(component.selectedBot).toBe('Rebalancer');
    });

    it('should have fetched quote assets', async () => {
      expect(component).toBeTruthy();
      expect(mockDataService.getQuoteAssetsSuitableForRebalancing).toHaveBeenCalled();
    });

    it('should fetch available base assets on quote asset change', () => {
      component.selectedQuoteAsset = 'USDT';
      component.onQuoteAssetChange();
      expect(mockDataService.getAvailableBaseAssets).toHaveBeenCalledWith('USDT');
    });

    it('should delete a base asset config', () => {
      component.selectedBaseAssetsConfig = [
        { assetSymbol: 'BTC', expectedShare: 60 },
        { assetSymbol: 'ETH', expectedShare: 40 },
      ];

      expect(component.selectedBaseAssetsConfig.length).toBe(2);
      component.deleteSelectedBaseAssetConfig(component.selectedBaseAssetsConfig[0]);
      expect(component.selectedBaseAssetsConfig.length).toBe(1);
      expect(component.selectedBaseAssetsConfig[0].assetSymbol).toBe('ETH');
    });

    it('should add a base asset config after dialog closes with result', () => {
      component.availableBaseAssets = ['BTC', 'ETH'];
      expect(component.selectedBaseAssetsConfig.length).toBe(0);

      const afterClosedSpy = jasmine.createSpyObj('afterClosed', ['subscribe']);
      afterClosedSpy.subscribe.and.callFake((fn: (result: any) => void) => fn({ assetSymbol: 'BTC', assetShare: 70 }));
      mockDialog.open.and.returnValue({ afterClosed: () => afterClosedSpy } as any);

      component.addBaseAsset();

      expect(component.selectedBaseAssetsConfig.length).toBe(1);
      expect(component.selectedBaseAssetsConfig[0]).toEqual({ assetSymbol: 'BTC', expectedShare: 70 });
    });

    it('should validate config and show summary', () => {
      expect(component.isConfigValid).toBeFalse();
      expect(component.baseAssetConfigSummary).toContain('Execution period is needed');

      component.executionPeriodInput = '60';
      component.selectedBaseAssetsConfig = [{ assetSymbol: 'BTC', expectedShare: 100 }];
      expect(component.baseAssetConfigSummary).toContain('At least two base assets');
      expect(component.isConfigValid).toBeFalse();

      component.selectedBaseAssetsConfig = [
        { assetSymbol: 'BTC', expectedShare: 60 },
        { assetSymbol: 'ETH', expectedShare: 30 },
      ];
      expect(component.isConfigValid).toBeFalse();
      expect(component.baseAssetConfigSummary).toContain('The total share must be equal to 100 percent.');

      component.selectedBaseAssetsConfig = [
        { assetSymbol: 'BTC', expectedShare: 60 },
        { assetSymbol: 'ETH', expectedShare: 40 },
      ];
      component.selectedQuoteAsset = 'USDT';
      expect(component.baseAssetConfigSummary).toContain('The bot can be launched.');
      expect(component.isConfigValid).toBeTrue();
    });

    describe('when Rebalancer bot configuration is correct', async () => {
      beforeEach(async () => {
        component.selectedBot = 'Rebalancer';
        component.executionPeriodInput = '60';
        component.isExecutedImmediately = true;
        component.selectedQuoteAsset = 'USDT';
        component.selectedBaseAssetsConfig = [
          { assetSymbol: 'BTC', expectedShare: 60 },
          { assetSymbol: 'ETH', expectedShare: 40 },
        ];
        await triggerComponentInitAndStabilization();
      });

      it('should call startBot and show notification on failure', async () => {
        mockDataService.startBot.and.returnValue(Promise.resolve(RequestResult.Fail));
        await component.startBot();

        expect(mockDataService.startBot).toHaveBeenCalled();
        expect(mockNotificationService.show).toHaveBeenCalledWith('Failed to launch the bot', 3000, jasmine.anything());
      });

      it('should call startBot and not show notification on success', async () => {
        mockDataService.startBot.and.returnValue(Promise.resolve(RequestResult.Success));
        await component.startBot();
        expect(mockDataService.startBot).toHaveBeenCalled();
        expect(mockNotificationService.show).not.toHaveBeenCalled();
      });
    });
  });
});
