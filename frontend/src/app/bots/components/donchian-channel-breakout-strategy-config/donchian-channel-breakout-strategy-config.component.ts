import { CommonModule } from '@angular/common';
import { ChangeDetectorRef, Component, ViewChild } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { MatButtonModule } from '@angular/material/button';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatDialog, MatDialogModule } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatMenuModule } from '@angular/material/menu';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatSelectModule } from '@angular/material/select';
import { MatSort, MatSortModule } from '@angular/material/sort';
import { MatTableDataSource, MatTableModule } from '@angular/material/table';
import { BotName } from '@app/bots/bot-name-enum';
import { AddAssetAndNumberDialog } from '@app/bots/components/add-asset-and-number-dialog/add-asset-and-number-dialog.component';
import { DataService, RequestResult } from '@app/services/data.service';
import { NotificationService } from '@app/services/notification.service';
import { NotificationSeverity } from '@app/shared/components/notification/notification-severity-enum';

@Component({
  selector: 'app-donchian-channel-breakout-strategy-config',
  standalone: true,
  imports: [
    CommonModule,
    FormsModule,
    MatButtonModule,
    MatCheckboxModule,
    MatDialogModule,
    MatFormFieldModule,
    MatIconModule,
    MatInputModule,
    MatMenuModule,
    MatProgressSpinnerModule,
    MatSelectModule,
    MatSortModule,
    MatTableModule,
  ],
  templateUrl: './donchian-channel-breakout-strategy-config.component.html',
  styleUrls: ['./donchian-channel-breakout-strategy-config.component.scss'],
})
export class DonchianChannelBreakoutStrategyConfigComponent {
  public executionIntervalsInputFieldLabel = 'Execution interval';
  public readonly availableExecutionIntervals: string[] = [
    'OneMinute',
    'ThreeMinutes',
    'FiveMinutes',
    'FifteenMinutes',
    'ThirtyMinutes',
    'OneHour',
    'TwoHours',
    'FourHours',
    'SixHours',
    'EightHours',
    'TwelveHours',
    'OneDay',
    'ThreeDays',
    'OneWeek',
    'OneMonth',
  ];
  public selectedExecutionInterval = '';

  public exitChannelLengthLabel = 'Exit (sell) channel length';
  public exitChannelLength = '';
  public entryChannelLengthLabel = 'Entry (buy) channel length';
  public entryChannelLength = '';

  public availableQuoteAssets: string[] = [];
  public selectedQuoteAsset = '';
  public areAvailableQuoteAssetsLoading = true;

  public availableBaseAssets: string[] = [];
  public selectedBaseAssets: string[] = [];
  public areAvailableBaseAssetsLoading = false;

  public displayedSelectedBaseAssetsTableColumns: string[] = ['assetSymbol'];

  dataSource = new MatTableDataSource(this.selectedBaseAssets);
  @ViewChild(MatSort) sort!: MatSort;

  constructor(
    private dataService: DataService,
    private dialog: MatDialog,
    private notificationService: NotificationService,
    private cdr: ChangeDetectorRef,
  ) {}

  ngOnInit(): void {
    this.fetchAvailableQuoteAssets();
  }

  public onQuoteAssetChange() {
    this.fetchAvailableBaseAssets();
  }

  public updateSelectedBaseAssetConfigTable() {
    this.dataSource = new MatTableDataSource(this.selectedBaseAssets);
    this.dataSource.sort = this.sort;
    this.cdr.markForCheck();
  }

  public get configTitle(): string {
    return 'Donchian Channel Breakout Strategy configuration';
  }

  public fetchAvailableQuoteAssets(): void {
    this.areAvailableQuoteAssetsLoading = true;
    this.dataService.getQuoteAssetsSuitableForRebalancing().then((data) => {
      if (data) {
        this.availableQuoteAssets = data;
        this.areAvailableQuoteAssetsLoading = false;
        this.cdr.markForCheck();
      }
    });
  }

  public fetchAvailableBaseAssets(): void {
    this.areAvailableBaseAssetsLoading = true;

    this.dataService.getAvailableBaseAssets(this.selectedQuoteAsset).then((data) => {
      if (data) {
        this.availableBaseAssets = data;
        this.areAvailableBaseAssetsLoading = false;
        this.cdr.markForCheck();
      }
    });
  }

  public deleteSelectedBaseAsset(elementToRemove: string) {
    this.selectedBaseAssets = this.selectedBaseAssets.filter((elem: string) => {
      return elem !== elementToRemove;
    });
    this.updateSelectedBaseAssetConfigTable();
  }

  public get isQuoteAssetSelected(): boolean {
    return this.selectedQuoteAsset !== '';
  }

  public addBaseAsset(): void {
    const dialogRef = this.dialog.open(AddAssetAndNumberDialog, {
      width: '60vw',
      data: {
        title: 'Add base asset',
        inputFieldLabel: 'Base asset',
        assets: this.availableBaseAssets.filter((elem: string) => {
          return !this.selectedBaseAssets.includes(elem);
        }),
        showNumberInput: false,
      },
    });

    dialogRef.afterClosed().subscribe((result) => {
      console.log('Dialog closed with result:', result);
      if (result) {
        this.selectedBaseAssets.push(result.assetSymbol);
        this.updateSelectedBaseAssetConfigTable();
      }
    });
  }

  public get isConfigValid(): boolean {
    const isValidPositiveNumber = (value: string): boolean => {
      const num = Number(value);
      return value !== '' && !isNaN(num) && num > 0;
    };

    return (
      this.selectedExecutionInterval !== '' &&
      isValidPositiveNumber(this.exitChannelLength) &&
      isValidPositiveNumber(this.entryChannelLength) &&
      this.selectedQuoteAsset !== '' &&
      this.selectedBaseAssets.length >= 1
    );
  }

  public get configSummary(): string {
    if (this.selectedExecutionInterval === '') {
      return 'Execution interval is needed to launch the bot';
    }
    if (this.exitChannelLength === '' || this.entryChannelLength === '') {
      return 'Both channel lengths are needed to launch the bot';
    }
    if (this.selectedBaseAssets.length < 1) {
      return 'At least one base asset is needed to launch the bot';
    }
    if (!this.isConfigValid) {
      return 'Config is invalid; unexpected error occurred';
    }
    return 'Configuration is valid. The bot can be launched.';
  }

  public startBot(): void {
    this.dataService.startBot(this.buildBotParams()).then((result) => {
      if (result === RequestResult.Success) {
        console.log('Bot started successfully');
      } else if (result === RequestResult.Fail) {
        this.notificationService.show('Failed to launch the bot', 3000, NotificationSeverity.Error);
      }
    });
  }

  public getExecutionIntervalDisplayName(executionInterval: string): string {
    switch (executionInterval) {
      case 'OneMinute':
        return '1 Minute';
      case 'ThreeMinutes':
        return '3 Minutes';
      case 'FiveMinutes':
        return '5 Minutes';
      case 'FifteenMinutes':
        return '15 Minutes';
      case 'ThirtyMinutes':
        return '30 Minutes';
      case 'OneHour':
        return '1 Hour';
      case 'TwoHours':
        return '2 Hours';
      case 'FourHours':
        return '4 Hours';
      case 'SixHours':
        return '6 Hours';
      case 'EightHours':
        return '8 Hours';
      case 'TwelveHours':
        return '12 Hours';
      case 'OneDay':
        return '1 Day';
      case 'ThreeDays':
        return '3 Days';
      case 'OneWeek':
        return '1 Week';
      case 'OneMonth':
        return '1 Month';
      default:
        return executionInterval;
    }
  }

  private buildBotParams() {
    return {
      botName: BotName.DonchianChannelBreakoutStrategy,
      executionInterval: this.selectedExecutionInterval,
      exitChannelLength: Number(this.exitChannelLength),
      entryChannelLength: Number(this.entryChannelLength),
      quoteAsset: this.selectedQuoteAsset,
      baseAssets: this.selectedBaseAssets,
    };
  }
}
