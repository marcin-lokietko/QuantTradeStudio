import { ComponentFixture, TestBed } from '@angular/core/testing';
import { DataService } from '@app/services/data.service';
import { MatButtonModule } from '@angular/material/button';
import { MatDialog } from '@angular/material/dialog';
import { NotificationService } from '@app/services/notification.service';
import { BotsLaunchPage } from './bots-launch-page.component';
import { BotName } from '@app/bots/bot-name-enum';

describe('BotsLaunchPage', () => {
  let component: BotsLaunchPage;
  let fixture: ComponentFixture<BotsLaunchPage>;

  let mockDialog: jasmine.SpyObj<MatDialog>;
  let mockNotificationService: jasmine.SpyObj<NotificationService>;

  beforeEach(async () => {
    mockDialog = jasmine.createSpyObj('MatDialog', ['open']);
    mockNotificationService = jasmine.createSpyObj('NotificationService', ['show']);

    await TestBed.configureTestingModule({
      imports: [BotsLaunchPage, MatButtonModule],
      providers: [
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

  it('should show RebalancerConfigComponent when Rebalancer is selected', async () => {
    await triggerComponentInitAndStabilization();

    component.selectBot(BotName.Rebalancer);
    fixture.detectChanges();

    const compiled = fixture.nativeElement;
    const rebalancerConfig = compiled.querySelector('app-rebalancer-config');
    expect(rebalancerConfig).toBeTruthy();
  });
});
