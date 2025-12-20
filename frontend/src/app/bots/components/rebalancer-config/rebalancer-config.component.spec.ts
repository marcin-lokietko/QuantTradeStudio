import { ComponentFixture, TestBed } from '@angular/core/testing';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { DataService, Order, RequestResult } from '@app/services/data.service';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';
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
import { RebalancerConfigComponent } from './rebalancer-config.component';

describe('RebalancerConfigComponent', () => {
  let component: RebalancerConfigComponent;
  let fixture: ComponentFixture<RebalancerConfigComponent>;

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
      imports: [
        RebalancerConfigComponent,
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
        { provide: NotificationService, useValue: mockNotificationService },
      ],
    })
      // Not enough to mock MatDialog in "providers". MatDialog has many dependencies (like Overlay, Injector, etc.). Those dependencies are available in the TestBed
      // configuration (through the imported Material modules). Because of this the Angular can create a real MatDialog, ignoring the mock.
      // To prevent this we override the component to provide the mock MatDialog directly to the component.
      // This forces Angular to inject the mock into the component, even though it could technically create a real instance.
      .overrideComponent(RebalancerConfigComponent, {
        set: {
          providers: [{ provide: MatDialog, useValue: mockDialog }],
        },
      })
      .compileComponents();

    fixture = TestBed.createComponent(RebalancerConfigComponent);
    component = fixture.componentInstance;

    mockDataService.getQuoteAssetsSuitableForRebalancing.and.returnValue(Promise.resolve(['USDT', 'EUR']));
    mockDataService.getAvailableBaseAssets.and.returnValue(Promise.resolve(['BTC', 'ETH']));

    await triggerComponentInitAndStabilization();
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
    mockDialog;
    expect(component).toBeTruthy();
  });

  it('should fetch quote assets', async () => {
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

  it('should add base asset config after dialog closes with result', () => {
    console.log('mockDialog:', mockDialog);
    console.log('component dialog:', (component as any).dialog);
    component.availableBaseAssets = ['BTC', 'ETH'];
    expect(component.selectedBaseAssetsConfig.length).toBe(0);

    const afterClosedSpy = jasmine.createSpyObj('afterClosed', ['subscribe']);
    afterClosedSpy.subscribe.and.callFake((fn: (result: any) => void) => fn({ assetSymbol: 'BTC', number: 70 }));
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

  describe('when bot configuration is correct', async () => {
    beforeEach(async () => {
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
