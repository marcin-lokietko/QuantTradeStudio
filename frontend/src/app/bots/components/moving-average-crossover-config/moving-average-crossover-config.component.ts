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
  selector: 'app-moving-average-crossover-config',
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
  templateUrl: './moving-average-crossover-config.component.html',
  styleUrls: ['./moving-average-crossover-config.component.scss'],
})
export class MovingAverageCrossoverConfigComponent {
  public executionPeriodInputFieldLabel = 'Execution period in seconds';
  public executionPeriodInput = '';

  public shortTermMovingAverageLengthLabel = 'Short-term moving average length';
  public shortTermMovingAverageLength = '';
  public longTermMovingAverageLengthLabel = 'Long-term moving average length';
  public longTermMovingAverageLength = '';

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
    return 'Moving Average Crossover configuration';
  }

  public fetchAvailableQuoteAssets(): void {
    this.areAvailableQuoteAssetsLoading = true;
    this.dataService.getQuoteAssetsSuitableForBots().then((data) => {
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
      isValidPositiveNumber(this.executionPeriodInput) &&
      isValidPositiveNumber(this.shortTermMovingAverageLength) &&
      isValidPositiveNumber(this.longTermMovingAverageLength) &&
      Number(this.shortTermMovingAverageLength) < Number(this.longTermMovingAverageLength) &&
      this.selectedQuoteAsset !== '' &&
      this.selectedBaseAssets.length >= 1
    );
  }

  public get configSummary(): string {
    if (this.executionPeriodInput === '') {
      return 'Execution period is needed to launch the bot';
    }
    if (this.shortTermMovingAverageLength === '' || this.longTermMovingAverageLength === '') {
      return 'Both moving average lengths are needed to launch the bot';
    }
    if (Number(this.shortTermMovingAverageLength) >= Number(this.longTermMovingAverageLength)) {
      return 'Short-term moving average length must be less than long-term moving average length';
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

  private buildBotParams() {
    return {
      botName: BotName.MovingAverageCrossover,
      executionPeriod: Number(this.executionPeriodInput),
      shortTermMovingAverageLength: Number(this.shortTermMovingAverageLength),
      longTermMovingAverageLength: Number(this.longTermMovingAverageLength),
      quoteAsset: this.selectedQuoteAsset,
      baseAssets: this.selectedBaseAssets,
    };
  }
}
