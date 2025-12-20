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
import { DonchianChannelBreakoutStrategyConfigComponent } from './donchian-channel-breakout-strategy-config.component';

describe('DonchianChannelBreakoutStrategyConfigComponent', () => {
  let component: DonchianChannelBreakoutStrategyConfigComponent;
  let fixture: ComponentFixture<DonchianChannelBreakoutStrategyConfigComponent>;

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
        DonchianChannelBreakoutStrategyConfigComponent,
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
      .overrideComponent(DonchianChannelBreakoutStrategyConfigComponent, {
        set: {
          providers: [{ provide: MatDialog, useValue: mockDialog }],
        },
      })
      .compileComponents();

    fixture = TestBed.createComponent(DonchianChannelBreakoutStrategyConfigComponent);
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

  it('should delete base asset config', () => {
    component.selectedBaseAssets = ['BTC', 'ETH'];

    expect(component.selectedBaseAssets.length).toBe(2);
    component.deleteSelectedBaseAsset(component.selectedBaseAssets[0]);
    expect(component.selectedBaseAssets.length).toBe(1);
    expect(component.selectedBaseAssets[0]).toBe('ETH');
  });

  it('should add base asset config after dialog closes with result', () => {
    console.log('mockDialog:', mockDialog);
    console.log('component dialog:', (component as any).dialog);
    component.availableBaseAssets = ['BTC', 'ETH'];
    expect(component.selectedBaseAssets.length).toBe(0);

    const afterClosedSpy = jasmine.createSpyObj('afterClosed', ['subscribe']);
    afterClosedSpy.subscribe.and.callFake((fn: (result: any) => void) => fn({ assetSymbol: 'BTC' }));
    mockDialog.open.and.returnValue({ afterClosed: () => afterClosedSpy } as any);

    component.addBaseAsset();

    expect(component.selectedBaseAssets.length).toBe(1);
    expect(component.selectedBaseAssets[0]).toEqual('BTC');
  });

  it('should validate config and show summary', () => {
    expect(component.isConfigValid).toBeFalse();
    expect(component.configSummary).toContain('Execution interval is needed');

    component.selectedExecutionInterval = 'ThirtyMinutes';
    expect(component.configSummary).toContain('Both channel lengths are needed ');
    expect(component.isConfigValid).toBeFalse();
    component.exitChannelLength = '4';
    expect(component.configSummary).toContain('Both channel lengths are needed ');
    expect(component.isConfigValid).toBeFalse();

    component.entryChannelLength = '2';
    expect(component.configSummary).toContain('At least one base asset is needed');
    expect(component.isConfigValid).toBeFalse();

    component.selectedBaseAssets = ['BTC'];
    component.selectedQuoteAsset = 'USDT';
    expect(component.configSummary).toContain('The bot can be launched.');
    expect(component.isConfigValid).toBeTrue();
  });

  describe('when bot configuration is correct', async () => {
    beforeEach(async () => {
      component.selectedExecutionInterval = 'ThirtyMinutes';
      component.entryChannelLength = '4';
      component.exitChannelLength = '2';
      component.selectedQuoteAsset = 'USDT';
      component.selectedBaseAssets = ['BTC', 'ETH'];

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
